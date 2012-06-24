// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_CACHE_HPP_
#define _CHAI_KERNEL_CACHE_HPP_

#include <map>
#include <string>
#include <vector>

#include "KernelUnit.hpp"
#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// kernel cache

class KernelCache
{
    OCLdevice& _oclDevice;

    // canonical kernel name -> OpenCL kernel object
    std::map< std::string, OCLkernel* > _nameToKernelObj;

    // canonical kernel name -> OpenCL kernel source
    std::map< std::string, std::vector< std::string> > _nameToKernelSrc;

public:
    KernelCache(OCLdevice&);
    ~KernelCache(void);

    OCLkernel* factoryLookup(KernelUnit& func);
};

}; // namespace chai_internal

#endif
