// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <algorithm>
#include <errno.h>
#include <omp.h>
#include <stdint.h>
#include <sys/time.h>

#include "Scheduler.hpp"
#include "VectorTrace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// cleanup

void Scheduler::cleanup(void)
{
    for (map< pthread_t, ArrayClient* >::const_iterator
         it = _handles.begin();
         it != _handles.end();
         it++) { delete (*it).second; }
    _handles.clear();

    for (map< pthread_t, SingleTrace* >::const_iterator
         it = _traceMap.begin();
         it != _traceMap.end();
         it++) { delete (*it).second; }
    _traceMap.clear();
}

////////////////////////////////////////
// scheduling thread event loop

void* Scheduler::bossLoopThr(void* thisObj)
{
    Scheduler* pObj = static_cast<Scheduler*>(thisObj);
    pObj->bossLoopFun();
    return NULL;
}

void Scheduler::bossLoopFun(void)
{
    while (_bossLoop)
    {
        map< uint64_t,
             set< pair< pthread_t, SingleTrace* > >
             > currentWork;

        // collate client traces by hash code
/*TRACE_LOCK*/ pthread_mutex_lock(&_traceMtx);
        for (map< pthread_t, SingleTrace* >::iterator
             it = _traceMap.begin();
             it != _traceMap.end();
             it++)
        {
            if ((*it).second)
            {
                currentWork[ (*it).second->hashCode() ]
                    .insert( pair< pthread_t, SingleTrace* >(
                                 (*it).first,
                                 (*it).second) );
                (*it).second = NULL;
            }
        }
/*TRACE_UNLOCK*/ pthread_mutex_unlock(&_traceMtx);

        // add to pending work
/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);
        for (map< uint64_t,
                  set< pair< pthread_t, SingleTrace* > >
                  >::const_iterator
             it = currentWork.begin();
             it != currentWork.end();
             it++)
        {
            _workMap[ (*it).first ]
                .insert( (*it).second.begin(),
                         (*it).second.end() );
        }
        pthread_cond_broadcast(&_workCond);
/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

/*BOSS_LOCK*/ pthread_mutex_lock(&_bossMtx);
        if (0 == _bossCnt)
            pthread_cond_wait(&_bossCond, &_bossMtx);
        else
            _bossCnt--;
/*BOSS_UNLOCK*/ pthread_mutex_unlock(&_bossMtx);
    }
}

////////////////////////////////////////
// device threads

void* Scheduler::deviceLoopThr(void* deviceCode)
{
    Scheduler::obj().deviceLoopFun(reinterpret_cast< size_t >(deviceCode));
    return NULL;
}

void Scheduler::deviceLoopFun(const size_t deviceCode)
{
    // stops deferral deadlock
    bool deferOverrideFlag = false;

    while (_deviceLoop)
    {
        map< uint64_t,
             pair< set< pthread_t >, set< SingleTrace* > >
             > workSelected;

        map< uint64_t,
             set< pair< pthread_t, SingleTrace* > >
             > workRestore;

        // for deferral deadlock detection
        size_t chooseWorkCount = 0;
        bool   deferWorkFlag   = false;

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);

        // look for work
        for (map< uint64_t,
                  set< pair< pthread_t, SingleTrace* > >
                  >::const_iterator
             it = _workMap.begin();
             it != _workMap.end();
             it++)
        {
            const uint64_t hashCode = (*it).first;

            const pair< bool, bool > takeOrDefer
                = deviceLoopChooseWork( deviceCode,
                                        hashCode,
                                        deferOverrideFlag );

            if ( takeOrDefer.first )
            {
                chooseWorkCount++;

                set< pthread_t > pthSet;
                set< SingleTrace* > traceSet;

                // copy work
                for (set< pair< pthread_t, SingleTrace* > >::const_iterator
                     jt = (*it).second.begin();
                     jt != (*it).second.end();
                     jt++)
                {
                    pthSet.insert((*jt).first);
                    traceSet.insert((*jt).second);

                    // keep for restore if compute device fails
                    workRestore[hashCode]
                        .insert(
                            pair< pthread_t, SingleTrace* >(
                                (*jt).first,
                                (*jt).second) );
                }

                // add to selected work
                workSelected[hashCode]
                    = pair< set< pthread_t >,
                            set< SingleTrace* >
                            >( pthSet, traceSet );
            }
            else
            {
                // was the work deferred?
                if ( takeOrDefer.second )
                    deferWorkFlag = true;
            }
        }

        // reset the override so it only works once
        deferOverrideFlag = false;

        // remove work so other devices do not see it
        for (map< uint64_t,
                  pair< set< pthread_t >, set< SingleTrace* > >
                  >::const_iterator
             it = workSelected.begin();
             it != workSelected.end();
             it++)
        {
            _workMap.erase((*it).first);
        }

/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

        // process the checked out work
        for (map< uint64_t,
                  pair< set< pthread_t >, set< SingleTrace* > >
                  >::const_iterator
             it = workSelected.begin();
             it != workSelected.end();
             it++)
        {
            const uint64_t hashCode = (*it).first;

            const set< pthread_t >& handleSet = (*it).second.first;
            const set< SingleTrace* >& traceSet = (*it).second.second;

            // VECTOR TRACE
            VectorTrace vt(hashCode, traceSet);

            // EXECUTOR (blocks and waits for the compute device)
            const double goodnessTime = _executor.dispatch(deviceCode, vt);

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);

            // add to history
            _workHist[hashCode]
                .insert(
                    handleSet.begin(),
                    handleSet.end() );

            // update statistics
            if (_workStat[hashCode].count(deviceCode))
            {
                // device has run this kernel before
                const double previousVal = _workStat[hashCode][deviceCode];

                // update goodness time if faster (or failed)
                // FIXME - this should really be a median time
                //         also, a pessimistic view means the
                //         goodness time should be the worst
                //         case, not the best

                // note that failure codes are negative, so if a device
                // works ok but then fails, it will be marked as failing
                _workStat[hashCode][deviceCode]
                    = goodnessTime < previousVal
                          ? goodnessTime
                          : previousVal;
            }
            else
            {
                // first time for this device
                _workStat[hashCode][deviceCode] = goodnessTime;
            }

            // if compute device did not do the work, must add it back
            if (goodnessTime <= _executor.DEVICE_FAILURE)
            {
                for (set< pair< pthread_t, SingleTrace* > >::const_iterator
                     jt = workRestore[hashCode].begin();
                     jt != workRestore[hashCode].end();
                     jt++)
                {
                    _workMap[hashCode].insert(*jt);
                }
            }

/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

            // cleanup and signal client threads
            if (goodnessTime > _executor.DEVICE_FAILURE)
            {
/*TRACE_LOCK*/ pthread_mutex_lock(&_traceMtx);

                // finished with traces, discard them
                for (set< SingleTrace* >::const_iterator
                     jt = traceSet.begin();
                     jt != traceSet.end();
                     jt++)
                {
                    _traceRef.release(*jt);
                }

                // remove entries for client threads
                for (set< pthread_t >::const_iterator
                     jt = handleSet.begin();
                     jt != handleSet.end();
                     jt++)
                {
                    _traceMap.erase(*jt);
                }

                pthread_cond_broadcast(&_traceCond);

/*TRACE_UNLOCK*/ pthread_mutex_unlock(&_traceMtx);
            }
        }

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);

        // timed wait
        struct timeval tval;
        gettimeofday(&tval, NULL);

        struct timespec tspec;
        tspec.tv_sec = tval.tv_sec;
        tspec.tv_nsec = tval.tv_usec * 1000;

        // delay should auto-tune, exceed longest kernel time for device
        const size_t delay = 500 * 1000 * 1000; // .5 sec
        tspec.tv_nsec += delay;
        if (tspec.tv_nsec < delay)
            tspec.tv_sec++;

        const int rc = pthread_cond_timedwait(&_workCond,
                                              &_workMtx,
                                              &tspec);

        // check for deferral deadlock
        if (ETIMEDOUT == rc && 0 == chooseWorkCount && deferWorkFlag)
        {
            deferOverrideFlag = true;
        }

/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

    } // while (_deviceLoop)
}

pair< bool, bool > Scheduler::deviceLoopChooseWork(
                                  const size_t deviceCode,
                                  const uint64_t hashCode,
                                  const bool deferOverride)
{
    bool doWork, deferWork = false;

    // is there any history for this kernel?
    if (_workHist.count(hashCode))
    {
        // is there any history for this device?
        if (_workStat[hashCode].count(deviceCode))
        {
            map< double, size_t > sortedTimes;

            // sort historical device timings
            for (map< size_t, double >::const_iterator
                 it = _workStat[hashCode].begin();
                 it != _workStat[hashCode].end();
                 it++)
            {
                // negative time means skip device
                if ((*it).second > _executor.DEVICE_FAILURE)
                    sortedTimes[(*it).second] = (*it).first;
            }

            // find fastest device
            const size_t fastestDevice = sortedTimes.empty()
                                             ? -1 /* invalid device code */
                                             : (*sortedTimes.begin()).second;

            // is this the fastest device?
            if (deviceCode == fastestDevice)
            {
                set< pthread_t > pthSet;

                // assemble thread set
                for (set< pair< pthread_t, SingleTrace* > >::const_iterator
                     it = _workMap[hashCode].begin();
                     it != _workMap[hashCode].end();
                     it++)
                {
                    pthSet.insert((*it).first);
                }

                // has thread set been seen before?
                if ( includes(
                         _workHist[hashCode].begin(),
                         _workHist[hashCode].end(),
                         pthSet.begin(),
                         pthSet.end()) )
                {
                    if (_workHist[hashCode].size() == pthSet.size())
                    {
                        // exact match, take the work
                        doWork = true;
                    }
                    else
                    {
                        // proper subset, defer work until later
                        deferWork = true;

                        if (deferOverride)
                        {
                            doWork = true;
                        }
                        else
                        {
                            doWork = false;
                        }
                    }
                }
                else
                {
                    // there are new threads, take the work
                    doWork = true;
                }
            }
            else
            {
                // another device is faster, skip
                doWork = false;
            }
        }
        else
        {
            // this device has never run this kernel, take the work
            doWork = true;
        }
    }
    else
    {
        // never seen this kernel before so take the work
        doWork = true;
    }

    return pair< bool, bool >( doWork, deferWork );
}

////////////////////////////////////////
// singleton

Scheduler::Scheduler(void)
    : _executor()
{
    pthread_mutex_init(&_mtx, NULL);
    pthread_mutex_init(&_traceMtx, NULL);
    pthread_mutex_init(&_bossMtx, NULL);
    pthread_mutex_init(&_workMtx, NULL);
    pthread_cond_init(&_traceCond, NULL);
    pthread_cond_init(&_bossCond, NULL);
    pthread_cond_init(&_workCond, NULL);

    // this should auto-tune
    omp_set_num_threads(256);
}

Scheduler::~Scheduler(void)
{
    pthread_mutex_destroy(&_mtx);
    pthread_mutex_destroy(&_traceMtx);
    pthread_mutex_destroy(&_bossMtx);
    pthread_mutex_destroy(&_workMtx);
    pthread_cond_destroy(&_traceCond);
    pthread_cond_destroy(&_bossCond);
    pthread_cond_destroy(&_workCond);

    cleanup();
}

Scheduler& Scheduler::obj(void)
{
    static Scheduler obj;
    return obj;
}

////////////////////////////////////////
// configuration

void Scheduler::init(void)
{
    // boss thread
    _bossLoop = true;
    _bossCnt = 0;
    pthread_create( &_boss,
                    NULL,
                    bossLoopThr,
                    &Scheduler::obj() );

    // device threads
    _deviceLoop = true;
    _workMap.clear();
    for (set< size_t >::const_iterator
         it = _executor.deviceCodes().begin();
         it != _executor.deviceCodes().end();
         it++)
    {
        const size_t deviceCode = *it;
        pthread_create( &_devicePth[deviceCode],
                        NULL,
                        deviceLoopThr,
                        reinterpret_cast< void* >(deviceCode) );
    }
}

void Scheduler::shutdown(void)
{
    void* status;

    // device threads
    _deviceLoop = false;
/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);
    pthread_cond_broadcast(&_workCond);
/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);
    for (set< size_t >::const_iterator
         it = _executor.deviceCodes().begin();
         it != _executor.deviceCodes().end();
         it++)
    {
        const size_t deviceCode = *it;
        pthread_join(_devicePth[deviceCode], &status);
    }
    _devicePth.clear();

    // boss thread
    _bossLoop = false;
/*BOSS_LOCK*/ pthread_mutex_lock(&_bossMtx);
    _bossCnt = -1; // don't wait
    pthread_cond_signal(&_bossCond);
/*BOSS_UNLOCK*/ pthread_mutex_unlock(&_bossMtx);
    pthread_join(_boss, &status);

    // clear client data
    cleanup();
}

ArrayClient* Scheduler::client(void)
{
    ArrayClient* ptr;

    const pthread_t ptself = pthread_self();

/*CLIENT_LOCK*/ pthread_mutex_lock(&_mtx);

    if (_handles.count(ptself))
        ptr = _handles[ptself];
    else
        ptr = _handles[ptself] = new ArrayClient(ptself);

/*CLIENT_UNLOCK*/ pthread_mutex_unlock(&_mtx);

    return ptr;
}

void Scheduler::wait(const pthread_t ptself, ClientTrace& ct)
{
    SingleTrace* tracePtr = new SingleTrace(ct);

/*TRACE_LOCK*/ pthread_mutex_lock(&_traceMtx);

    // enqueue
    _traceRef.checkout(tracePtr);
    _traceMap[ptself] = tracePtr;

    // increment client count, wake up boss
/*BOSS_LOCK*/ pthread_mutex_lock(&_bossMtx);
    _bossCnt++;
    pthread_cond_signal(&_bossCond);
/*BOSS_UNLOCK*/ pthread_mutex_unlock(&_bossMtx);

    // wait
    while (_traceMap.count(ptself))
        pthread_cond_wait(&_traceCond, &_traceMtx);

/*TRACE_UNLOCK*/ pthread_mutex_unlock(&_traceMtx);
}

}; // namespace chai_internal
