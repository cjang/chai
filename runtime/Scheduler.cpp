// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <algorithm>
#include <errno.h>

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <stdint.h>
#include <sys/time.h>

#include "Logger.hpp"
#include "OCLhacks.hpp"
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
        map< uint64_t, map< pthread_t, SingleTrace* > > currentWork;

/*TRACE_LOCK*/ pthread_mutex_lock(&_traceMtx);
        // collate client traces by hash code
        for (map< pthread_t, SingleTrace* >::iterator
             it = _traceMap.begin();
             it != _traceMap.end();
             it++)
        {
            SingleTrace* sitr = (*it).second;

            if (sitr)
            {
                currentWork[ sitr->hashCode() ][ (*it).first ] = sitr;
                (*it).second = NULL;
            }
        }
/*TRACE_UNLOCK*/ pthread_mutex_unlock(&_traceMtx);

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);
        // add to pending work
        for (map< uint64_t, map< pthread_t, SingleTrace* > >::const_iterator
             it = currentWork.begin();
             it != currentWork.end();
             it++)
        {
            _workMap[ (*it).first ].insert( (*it).second.begin(),
                                            (*it).second.end() );
        }

        pthread_cond_broadcast(&_workCond);
/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

/*BOSS_LOCK*/ pthread_mutex_lock(&_bossMtx);
        // wait or look again for work
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
    Scheduler::singleton()
        .deviceLoopFun(reinterpret_cast< size_t >(deviceCode));

    return NULL;
}

void Scheduler::deviceLoopFun(const size_t deviceCode)
{
    // stops deferral deadlock
    bool deferOverrideFlag = false;

    while (_deviceLoop)
    {
        if (deviceCode >= MemManager::GPU_OPENCL)
        {
/*IDLEGPU_LOCK*/ pthread_mutex_lock(&_deviceMtx);
            _idleGPUCnt--;
/*IDLEGPU_UNLOCK*/ pthread_mutex_unlock(&_deviceMtx);
        }

        // for deferral deadlock detection
        size_t chooseWorkCount = 0;
        bool   deferWorkFlag   = false;

        // selected work, will be restored to queue if device fails
        map< uint64_t, map< pthread_t, SingleTrace* > > workSelected;

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);

        // look for work
        for (map< uint64_t, map< pthread_t, SingleTrace* > >::const_iterator
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

                // copy work not stuck to another device
                for (map< pthread_t, SingleTrace* >::const_iterator
                     jt = (*it).second.begin();
                     jt != (*it).second.end();
                     jt++)
                {
                    if ((*jt).second->stickyDevice( deviceCode ))
                        workSelected[ hashCode ][ (*jt).first ] = (*jt).second;
                }
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
        for (map< uint64_t, map< pthread_t, SingleTrace* > >::const_iterator
             it = workSelected.begin();
             it != workSelected.end();
             it++)
        {
            for (map< pthread_t, SingleTrace* >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                _workMap[ (*it).first ].erase( (*jt).first );
            }

            if (0 == _workMap[ (*it).first ].size())
                _workMap.erase((*it).first);
        }

/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

        // process the checked out work
        for (map< uint64_t, map< pthread_t, SingleTrace* > >::const_iterator
             it = workSelected.begin();
             it != workSelected.end();
             it++)
        {
            const uint64_t hashCode = (*it).first;
            const map< pthread_t, SingleTrace* >& traceSet = (*it).second;

#ifdef __LOGGING_ENABLED__
            for (map< pthread_t, SingleTrace* >::const_iterator
                 jt = traceSet.begin();
                 jt != traceSet.end();
                 jt++)
            {
                stringstream ss;
                ss << "thread=" << (*jt).first
                   << " hash=" << hashCode
                   << " device=" << deviceCode
                   << " sticky=" << (*jt).second->stickyDevice();
                LOGGER(ss.str())
            }
#endif

            // VECTOR TRACE
            VectorTrace vt(traceSet);

            // EXECUTOR (blocks and waits for the compute device)
            const double goodnessTime = _executor->dispatch(deviceCode, vt);

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);

            // add to history
            for (map< pthread_t, SingleTrace* >::const_iterator
                 jt = traceSet.begin();
                 jt != traceSet.end();
                 jt++)
            {
                _workHist[hashCode].insert((*jt).first);
            }

            // update statistics
            if (_workStat[hashCode].count(deviceCode))
            {
                // device has run this kernel before
                const double previousVal = _workStat[hashCode][deviceCode];

                // update goodness time if faster (or failed)
                // This should really be a median time.
                // Also, a pessimistic view means the goodness time should be
                // the worst case, not the best

                // note that failure codes are negative, so if a device
                // works ok but then fails, it will be marked as failing
                _workStat[hashCode][deviceCode] = goodnessTime < previousVal
                                                      ? goodnessTime
                                                      : previousVal;
            }
            else
            {
                // first time for this device
                _workStat[hashCode][deviceCode] = goodnessTime;
            }

            // if compute device did not do the work, must add it back
            if (goodnessTime <= _executor->DEVICE_FAILURE)
            {
                _workMap[hashCode].insert( traceSet.begin(),
                                           traceSet.end() );

                // remove device stickyness
                for (map< pthread_t, SingleTrace* >::const_iterator
                     jt = traceSet.begin();
                     jt != traceSet.end();
                     jt++)
                {
                    (*jt).second->unstickyDevice();
                }
            }

/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

            // cleanup and signal client threads
            if (goodnessTime > _executor->DEVICE_FAILURE)
            {
/*TRACE_LOCK*/ pthread_mutex_lock(&_traceMtx);

                for (map< pthread_t, SingleTrace* >::const_iterator
                     jt = traceSet.begin();
                     jt != traceSet.end();
                     jt++)
                {
                    // remove entries for client threads
                    _traceMap.erase((*jt).first);

                    // finished with traces, discard them
                    _traceRef.release((*jt).second);
                }

                pthread_cond_broadcast(&_traceCond);

/*TRACE_UNLOCK*/ pthread_mutex_unlock(&_traceMtx);
            }
        }

        if (deviceCode >= MemManager::GPU_OPENCL)
        {
/*IDLEGPU_LOCK*/ pthread_mutex_lock(&_deviceMtx);
            _idleGPUCnt++;
/*IDLEGPU_UNLOCK*/ pthread_mutex_unlock(&_deviceMtx);
        }

        // CPU interpreter has lowest priority
        bool waitForIdleGPUs = true;
        while (_deviceLoop && waitForIdleGPUs)
        {
            // timed wait
            struct timeval tval;
            gettimeofday(&tval, NULL);

            struct timespec tspec;
            tspec.tv_sec = tval.tv_sec;
            tspec.tv_nsec = tval.tv_usec * 1000;

            // delay should auto-tune, exceed longest kernel time for device
            const size_t delay = 100 * 1000 * 1000; // .1 sec
            tspec.tv_nsec += delay;
            if (tspec.tv_nsec < delay)
                tspec.tv_sec++;

/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);
            const int rc = pthread_cond_timedwait(&_workCond,
                                                  &_workMtx,
                                                  &tspec);
/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);

            // check for deferral deadlock
            if (ETIMEDOUT == rc && 0 == chooseWorkCount && deferWorkFlag)
            {
                deferOverrideFlag = true;
            }

            if (deviceCode < MemManager::GPU_OPENCL)
            {
/*IDLEGPU_LOCK*/ pthread_mutex_lock(&_deviceMtx);
                waitForIdleGPUs = _idleGPUCnt > 0;
/*IDLEGPU_UNLOCK*/ pthread_mutex_unlock(&_deviceMtx);
            }
            else
            {
                waitForIdleGPUs = false;
            }
        }

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
            // ...and has it ever failed for this kernel?
            if (_workStat[hashCode][deviceCode] > _executor->DEVICE_FAILURE)
            {
                // it worked before

                bool isOldThreads = true;
                size_t oldThreadCnt = 0;

                // has thread set been seen before?
                for (map< pthread_t, SingleTrace* >::const_iterator
                     it = _workMap[hashCode].begin();
                     it != _workMap[hashCode].end();
                     it++)
                {
                    if (0 == _workHist[hashCode].count((*it).first))
                    {
                        isOldThreads = false;
                        break;
                    }
                    else
                    {
                        oldThreadCnt++;
                    }
                }

                if (isOldThreads)
                {
                    if (_workHist[hashCode].size() == oldThreadCnt)
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
                // known history of failure with this device for the kernel
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

Scheduler::Scheduler()
    : _executor(NULL),
      _idleGPUCnt(0),
      _swizzleKey(0)
{
    pthread_mutex_init(&_mtx, NULL);
    pthread_mutex_init(&_traceMtx, NULL);
    pthread_mutex_init(&_bossMtx, NULL);
    pthread_mutex_init(&_workMtx, NULL);
    pthread_mutex_init(&_deviceMtx, NULL);
    pthread_mutex_init(&_swizzleMtx, NULL);
    pthread_cond_init(&_traceCond, NULL);
    pthread_cond_init(&_bossCond, NULL);
    pthread_cond_init(&_workCond, NULL);
}

Scheduler::~Scheduler(void)
{
    pthread_mutex_destroy(&_mtx);
    pthread_mutex_destroy(&_traceMtx);
    pthread_mutex_destroy(&_bossMtx);
    pthread_mutex_destroy(&_workMtx);
    pthread_mutex_destroy(&_deviceMtx);
    pthread_mutex_destroy(&_swizzleMtx);
    pthread_cond_destroy(&_traceCond);
    pthread_cond_destroy(&_bossCond);
    pthread_cond_destroy(&_workCond);

    cleanup();
}

Scheduler& Scheduler::singleton(void)
{
    static Scheduler obj;
    return obj;
}

////////////////////////////////////////
// configuration

void Scheduler::init(istream& configSpec)
{
    _executor = new Executor(configSpec);

    // boss thread
    _bossLoop = true;
    _bossCnt = 0;
    pthread_create( &_boss,
                    NULL,
                    bossLoopThr,
                    &Scheduler::singleton() );

    // device threads
    _deviceLoop = true;
    _workMap.clear();
    for (set< size_t >::const_iterator
         it = _executor->deviceCodes().begin();
         it != _executor->deviceCodes().end();
         it++)
    {
        const size_t deviceCode = *it;

        pthread_create( &_devicePth[deviceCode],
                        NULL,
                        deviceLoopThr,
                        reinterpret_cast< void* >(deviceCode) );

        if (deviceCode >= MemManager::GPU_OPENCL)
        {
/*IDLEGPU_LOCK*/ pthread_mutex_lock(&_deviceMtx);
            _idleGPUCnt++;
/*IDLEGPU_UNLOCK*/ pthread_mutex_unlock(&_deviceMtx);
        }
    }
}

void Scheduler::shutdown(void)
{
    // NVIDIA segfaults on shutdown, do not know why
    // Note this assumes init() and shutdown() are only called once by apps.
    if (OCLhacks::singleton().shutdownNOP())
        return;

    void* status;

    // device threads
    _deviceLoop = false;
/*WORK_LOCK*/ pthread_mutex_lock(&_workMtx);
    pthread_cond_broadcast(&_workCond);
/*WORK_UNLOCK*/ pthread_mutex_unlock(&_workMtx);
    for (set< size_t >::const_iterator
         it = _executor->deviceCodes().begin();
         it != _executor->deviceCodes().end();
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

    _idleGPUCnt = 0;

    delete _executor;
    _executor = NULL;
}

void Scheduler::extendLanguage(const uint32_t opCode,
                               const BaseInterp& interpHandler,
                               const BaseTrans& jitHandler)
{
    _executor->extendLanguage(opCode,
                              interpHandler,
                              jitHandler);
}

size_t Scheduler::swizzleKey(void)
{
/*SWIZZLE_LOCK*/ pthread_mutex_lock(&_swizzleMtx);

    const size_t uniqueKey = _swizzleKey++;

/*SWIZZLE_UNLOCK*/ pthread_mutex_unlock(&_swizzleMtx);

    return uniqueKey;
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
