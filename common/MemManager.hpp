// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_MANAGER_HPP_
#define _CHAI_MEM_MANAGER_HPP_

#include <cstddef>
#include <map>
#include <pthread.h>
#include <set>
#include <stdint.h>
#include <vector>

#include "BackMem.hpp"
#include "DeviceMem.hpp"
#include "FrontMem.hpp"
#include "OCLdevice.hpp"
#include "OCLinit.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// memory manager

class MemManager
{
    const size_t _deviceCode;
    OCLinit*     _oclInit;
    OCLdevice*   _oclDevice;

    // keep track of compute device memory for live variables
    pthread_mutex_t                  _memMutex;
    std::map< BackMem*, DeviceMem* > _backDev;
    std::map< DeviceMem*, BackMem* > _devBack;

public:
    // base device codes
    static const size_t CPU_INTERP = 100;
    static const size_t GPU_OPENCL = 1000;

    // interpreter does not have any OpenCL-ness
    MemManager(const size_t deviceCode);

    // translator/JIT uses OpenCL
    MemManager(const size_t deviceCode,
               OCLinit&,
               OCLdevice&);

    ~MemManager(void);

    size_t getDeviceCode(void) const;

    // used by scheduler (allocate backing parent memory)
    void memalloc(VectorTrace& vt);                    // before kernel
    void memfree(VectorTrace& vt, const bool freeMem); // after kernel

    // data movement from compute device back to CPU
    void swizzle(BackMem*);

    // backing memory does not need managing any more
    void untrackMem(BackMem*);

    // used by JIT dispatching
    BackMem* unifyBackMem(const uint32_t variable,
                          const uint32_t version,
                          std::vector< FrontMem* >&);

    // create memory on compute device (trace variable)
    bool createDevMem(VectorTrace& vt,
                      const uint32_t variableOnDevice,
                      BackMem* backMem,
                      const bool isReadTexture);
    bool createDevMem(VectorTrace& vt,
                      const uint32_t variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const bool isDP);
    bool createDevMem(VectorTrace& vt,
                      const uint32_t variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const float value);
    bool createDevMem(VectorTrace& vt,
                      const uint32_t variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const double value);

    // create memory on compute device (split variable)
    bool createDevMem(VectorTrace& vt,
                      AstVariable* variableOnDevice,
                      BackMem* backMem,
                      const bool isReadTexture);
    bool createDevMem(VectorTrace& vt,
                      AstVariable* variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const bool isDP);
    bool createDevMem(VectorTrace& vt,
                      AstVariable* variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const float value);
    bool createDevMem(VectorTrace& vt,
                      AstVariable* variableOnDevice,
                      const size_t W,
                      const size_t H,
                      const double value);

    // used by interpreter during operation dispatch
    BackMem* allocBackMem(const size_t W,
                          const size_t H,
                          const bool isDP,
                          const size_t vecSize);
    FrontMem* allocFrontMem(const size_t W,
                            const size_t H,
                            const bool isDP,
                            BackMem* parent,
                            const size_t vecIndex) const;
};

}; // namespace chai_internal

#endif
