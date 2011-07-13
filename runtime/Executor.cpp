// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <sys/time.h>

#include "Executor.hpp"

using namespace std;

namespace chai_internal {

Executor::Executor(void)
    : _oclInit(),
      _deviceMap(_oclInit),
      DEVICE_FAILURE(-1),
      TIMER_FAILURE(-10) { }

const set< size_t >& Executor::deviceCodes(void) const
{
    return _deviceMap.allCodes();
}

double Executor::dispatch(const size_t deviceCode,
                          VectorTrace& vt)
{
    struct timeval tvStart, tvStop;

    if (0 != gettimeofday(&tvStart, NULL))
    {
        // this really shouldn't happen
        cerr << "ERROR: gettimeofday(&tvStart, NULL) in executor" << endl;

        // failure - if this keeps happening, then nothing will be dispatched
        return TIMER_FAILURE;
    }

    // FIXME - The executor times the JIT as well as kernel execution.
    // This will almost always lose to the interpreter. Once the JIT
    // leaves a good solution in the kernel cache, then it's ok. But
    // as the JIT is always included, the compute device using it will
    // always lose.
    //
    // Solution is to break the evaluation into two halves. The top
    // half is the JIT which is not timed. The bottom half is the
    // kernel execution. Interpreters have a NOP top half.

    if (! _deviceMap.getDevice(deviceCode)->evaluate(vt))
    {
        return DEVICE_FAILURE; // failure
    }

    if (0 != gettimeofday(&tvStop, NULL))
    {
        // this really shouldn't happen
        cerr << "ERROR: gettimeofday(&tvStop, NULL) in executor" << endl;

        // device has already computed the trace but the timer failed
        return TIMER_FAILURE;
    }

    // calculate elapsed time
    const size_t elapsed_usecs
        = (tvStop.tv_sec - tvStart.tv_sec) * 1000000
          + (tvStop.tv_usec + 1000000 - tvStart.tv_usec)
          - 1000000;

    // normalize by amount of vectorization
    double goodnessTime = elapsed_usecs;
    goodnessTime /= vt.numTraces();

// FIXME
cerr << "goodnessTime: " << goodnessTime << endl;

    // The number of traces will vary and are a consequence of auto-tuning.
    // Just as indicated in the PeakStream architectural diagram, the
    // executor feeds into profiling. That's where the auto-tuning should
    // reside. (FIXME - have to do this later)

    return goodnessTime;
}

}; // namespace chai_internal
