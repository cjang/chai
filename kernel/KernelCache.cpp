// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>

#include "KernelCache.hpp"
#include "Logger.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// kernel cache

KernelCache::KernelCache(OCLdevice& oclDevice)
    : _oclDevice(oclDevice) { }

KernelCache::~KernelCache(void) { }

OCLkernel* KernelCache::factoryLookup(Function& func)
{
    // canonical kernel name
    const string kernelName = func.functionName();

    // check cache of kernel objects
    if (_nameToKernelObj.count(kernelName))
    {
        // kernel object found
        return _nameToKernelObj[ kernelName ];
    }

    // check cache of kernel source text
    if (_nameToKernelSrc.count(kernelName))
    {
        // still have source for this kernel

        // create new kernel object
        OCLkernel* ckernel = new OCLkernel(
                                     _oclDevice,
                                     kernelName,
                                     _nameToKernelSrc[kernelName] );

        // check if kernel compiled ok (it should as source is in the cache)
        if (ckernel->isOk())
        {
            // success
            _nameToKernelObj[kernelName] = ckernel;
            return ckernel;
        }
        else
        {
            // failure
            delete ckernel;
            return NULL;
        }
    }

    // must build a new custom kernel

    // print kernel function
    vector< string > sourceText;
    func.print(sourceText);

#ifdef __LOGGING_ENABLED__
    // print out generated custom kernel source text for debugging
    for (vector< string >::const_iterator
         it = sourceText.begin();
         it != sourceText.end();
         it++)
    {
        cerr << "OpenCL: " << *it;
    }
#endif

    // create kernel object
    OCLkernel* ckernel = new OCLkernel(
                                 _oclDevice,
                                 kernelName,
                                 sourceText );

    // check if kernel compiled ok (it should as source is in the cache)
    if (ckernel->isOk())
    {
        // success
        _nameToKernelSrc[kernelName] = sourceText;
        _nameToKernelObj[kernelName] = ckernel;
        return ckernel;
    }
    else
    {
        // failure
        delete ckernel;
        return NULL;
    }
}

}; // namespace chai_internal
