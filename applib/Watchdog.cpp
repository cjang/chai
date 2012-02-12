// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>
#include <time.h>

#include "Logger.hpp"
#include "Watchdog.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// workaround OpenCL compiler hangs

void* Watchdog::pollThr(void* thisObj)
{
    Watchdog* pObj = static_cast< Watchdog* >(thisObj);
    pObj->pollFun();
    return NULL;
}

void Watchdog::pollFun(void)
{
    while (_seconds)
    {
        struct timespec reqNS, remNS;

        // one second sleep
        reqNS.tv_sec = 1;
        reqNS.tv_nsec = 0;
        remNS.tv_sec = 0;
        remNS.tv_nsec = 0;

        // sleep loop
        while ( -1 == nanosleep(&reqNS, &remNS) &&
                remNS.tv_sec != 0 &&
                remNS.tv_nsec != 0 )
        {
            reqNS = remNS;
            remNS.tv_sec = 0;
            remNS.tv_nsec = 0;
        }

        // decrement counter and check for timeout
        pthread_mutex_lock(&_mtx);
        const bool isTimeout = 0 == _counter--;
        pthread_mutex_unlock(&_mtx);

        if (isTimeout)
        {
            LOGGER("watchdog timeout")
            exit(1);
        }
    }
}

Watchdog::Watchdog(void)
    : _seconds(0) // disabled
{
    pthread_mutex_init(&_mtx, NULL);
}

Watchdog::~Watchdog(void)
{
    pthread_mutex_destroy(&_mtx);
}

Watchdog& Watchdog::singleton(void)
{
    static Watchdog obj;
    return obj;
}

void Watchdog::enableDog(const size_t seconds)
{
    if (0 == _seconds)
    {
        _seconds = seconds;

        resetDog();

        pthread_create( &_pthPoll,
                        NULL,
                        pollThr,
                        &Watchdog::singleton() );
    }
}

void Watchdog::disableDog(void)
{
    if (0 != _seconds)
    {
        _seconds = 0;

        void* status;

        pthread_join( _pthPoll,
                      &status );
    }
}

void Watchdog::resetDog(void)
{ 
    pthread_mutex_lock(&_mtx);
    _counter = _seconds;
    pthread_mutex_unlock(&_mtx);
}

}; // namespace chai_internal
