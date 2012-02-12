// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_WATCHDOG_HPP_
#define _CHAI_WATCHDOG_HPP_

#include <pthread.h>
#include <sys/time.h>

namespace chai_internal {

////////////////////////////////////////
// workaround OpenCL compiler hangs

class Watchdog
{
    size_t          _seconds; // reset timeout for a hang event

    pthread_mutex_t _mtx;
    size_t          _counter; // hang if this ever reaches zero

    // watchdog thread polls the time and detects hangs
    pthread_t       _pthPoll;

    static void* pollThr(void*);
    void  pollFun(void);

    Watchdog(void);

public:
    ~Watchdog(void);

    static Watchdog& singleton(void);

    void enableDog(const size_t seconds);
    void disableDog(void);

    void resetDog(void);
};

}; // namespace chai_internal

#endif
