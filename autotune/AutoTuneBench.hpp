// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AUTO_TUNE_BENCH_HPP_
#define _CHAI_AUTO_TUNE_BENCH_HPP_

#include <ostream>

#include "AutoTuneMemo.hpp"
#include "KernelBase.hpp"
#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// autotuning controller

class AutoTuneBench
{
    OCLdevice&    _cdev;
    AutoTuneMemo& _journal;

    size_t runTime(const size_t trialNumber,
                   KernelBase& kernelGen,
                   const bool includeSendDataToDevice,
                   const bool includeReadDataFromDevice,
                   std::ostream* os);

public:
    AutoTuneBench(OCLdevice& cdev,
                  AutoTuneMemo& journal);

    size_t runTime(const size_t trialNumber,
                   KernelBase& kernelGen,
                   const bool includeSendDataToDevice,
                   const bool includeReadDataFromDevice);

    size_t runTime(const size_t trialNumber,
                   KernelBase& kernelGen,
                   const bool includeSendDataToDevice,
                   const bool includeReadDataFromDevice,
                   std::ostream& os);
};

}; // namespace chai_internal

#endif
