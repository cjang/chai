// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <sstream>

#include "KernelCache.hpp"
#include "Logger.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// kernel cache

KernelCache::KernelCache(OCLdevice& oclDevice)
    : _oclDevice(oclDevice) { }

KernelCache::~KernelCache(void) { }

OCLkernel* KernelCache::factoryLookup(KernelUnit& func)
{
    // kernel name and optional hash code
    stringstream ss(func.functionName());

    string kernelName;
    ss >> kernelName;

    uint64_t kernelHashCode = 0;
    if (! ss.eof()) ss >> kernelHashCode;

    string kernelKey;
    if (0 == kernelHashCode)
    {
        kernelKey = kernelName;
    }
    else
    {
        ss.str(string());
        ss << kernelName << kernelHashCode;
        kernelKey = ss.str();
    }

    // check cache of kernel objects
    if (_nameToKernelObj.count(kernelKey))
    {
        // kernel object found
        return _nameToKernelObj[ kernelKey ];
    }

    // check cache of kernel source text
    if (_nameToKernelSrc.count(kernelKey))
    {
        // still have source for this kernel

        // create new kernel object
        OCLkernel* ckernel = new OCLkernel(_oclDevice);
        ckernel->buildJIT(kernelName,
                          _nameToKernelSrc[kernelKey]);

        // check if kernel compiled ok (it should as source is in the cache)
        if (ckernel->isOk())
        {
            // success
            _nameToKernelObj[kernelKey] = ckernel;
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
         it = sourceText.begin(); it != sourceText.end(); it++)
    {
        cerr << "OpenCL: " << *it;
    }
#endif

    // create kernel object
    OCLkernel* ckernel = new OCLkernel(_oclDevice);
    if (0 == kernelHashCode)
    {
        ckernel->buildJIT(kernelName,
                          sourceText);
    }
    else
    {
        ckernel->buildCL(kernelName,
                         sourceText,
                         kernelHashCode);
    }

    // check if kernel compiled ok (it should as source is in the cache)
    if (ckernel->isOk())
    {
        // success
        _nameToKernelSrc[kernelKey] = sourceText;
        _nameToKernelObj[kernelKey] = ckernel;
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
