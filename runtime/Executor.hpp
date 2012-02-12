// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EXECUTOR_HPP_
#define _CHAI_EXECUTOR_HPP_

#include <cstddef>
#include <istream>
#include <set>
#include <stdint.h>

#include "BaseInterp.hpp"
#include "BaseTrans.hpp"
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

    Executor(std::istream& configSpec);

    // extend the language
    void extendLanguage(const uint32_t opCode,
                        const BaseInterp& interpHandler,
                        const BaseTrans& jitHandler);

    const std::set< size_t >& deviceCodes(void) const;

    double dispatch(const size_t deviceCode,
                    VectorTrace& traceVec);
};

}; // namespace chai_internal

#endif
