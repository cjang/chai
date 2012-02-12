// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SCHEDULER_HPP_
#define _CHAI_SCHEDULER_HPP_

#include <istream>
#include <map>
#include <pthread.h>
#include <set>
#include <stdint.h>
#include <utility>

#include "ArrayClient.hpp"
#include "BaseInterp.hpp"
#include "BaseTrans.hpp"
#include "chai/RefCnt.hpp"
#include "Executor.hpp"
#include "SingleTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// singleton per process
// centralized scheduler and dispatcher

class Scheduler
{
    // memory management embedded in device execution
    // OpenCL management embedded in device execution
    Executor* _executor;

    // array clients
    pthread_mutex_t                     _mtx;
    std::map< pthread_t, ArrayClient* > _handles;

    // client traces
    pthread_mutex_t                     _traceMtx;
    pthread_cond_t                      _traceCond;
    std::map< pthread_t, SingleTrace* > _traceMap;
    RefSet                              _traceRef;

    // boss scheduler thread
    pthread_t                           _boss;
    bool                                _bossLoop;
    pthread_mutex_t                     _bossMtx;
    pthread_cond_t                      _bossCond;
    size_t                              _bossCnt;
    static void* bossLoopThr(void* thisObj);
    void bossLoopFun(void);

    // device threads
    bool                                _deviceLoop;
    std::map< size_t, pthread_t >       _devicePth;
    static void* deviceLoopThr(void* deviceCode);
    void deviceLoopFun(const size_t deviceCode);
    std::pair< bool, bool > deviceLoopChooseWork(
                                const size_t deviceCode,
                                const uint64_t hashCode,
                                const bool deferOverride);

    // OpenCL/GPU compute devices prioritized before interpreter
    pthread_mutex_t _deviceMtx;
    size_t          _idleGPUCnt;

    // device work
    pthread_mutex_t _workMtx;
    pthread_cond_t  _workCond;

    // active work queue
    std::map< uint64_t, std::map< pthread_t, SingleTrace* > > _workMap;

    // work history
    std::map< uint64_t, std::set< pthread_t > >               _workHist;

    // work statistics
    std::map< uint64_t, std::map< size_t, double > >          _workStat;

    // unique swizzling keys to avoid redundant data transfers
    pthread_mutex_t _swizzleMtx;
    size_t          _swizzleKey;

    void cleanup(void);

    Scheduler(void);

public:
    ~Scheduler(void);

    static Scheduler& singleton(void);

    void init(std::istream& configSpec);
    void shutdown(void);

    // must be called after init(), otherwise the new clone handler objects
    // won't have a memory manager context
    void extendLanguage(const uint32_t opCode,
                        const BaseInterp& interpHandler,
                        const BaseTrans& jitHandler);

    size_t swizzleKey(void);

    ArrayClient* client(void);

    void wait(const pthread_t ptself, ClientTrace& ct);
};

}; // namespace chai_internal

#endif
