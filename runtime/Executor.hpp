// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EXECUTOR_HPP_
#define _CHAI_EXECUTOR_HPP_

#include <cstddef>
#include <set>

#include "DeviceMap.hpp"
#include "OCLinit.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// scheduler's portal to compute devices

class Executor
{
    OCLinit   _oclInit;
    DeviceMap _deviceMap;

public:
    const double DEVICE_FAILURE;
    const double TIMER_FAILURE;

    Executor(void);

    const std::set< size_t >& deviceCodes(void) const;

    double dispatch(const size_t deviceCode,
                    VectorTrace& traceVec);
};

}; // namespace chai_internal

#endif
