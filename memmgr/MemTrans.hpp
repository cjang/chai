// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_TRANS_HPP_
#define _CHAI_MEM_TRANS_HPP_

#include <cstddef>
#include <map>
#include <pthread.h>
#include <set>
#include <stdint.h>

#include "ArrayBuf.hpp"
#include "BackMem.hpp"
#include "FrontMem.hpp"
#include "KernelCache.hpp"
#include "MemAlloc.hpp"
#include "MemBase.hpp"
#include "MemCallback.hpp"
#include "OCLdevice.hpp"
#include "OCLinit.hpp"
#include "PrecType.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// memory manager for JIT

class MemTrans : public MemBase
{
    const int  _deviceNum;
    const bool _deviceIsCPU;

    OCLinit*   _oclInit;
    OCLdevice* _oclDevice;

    // kernel object and source text cache
    KernelCache* _cache;

    // keep track of compute device memory for live variable BackMem callback
    pthread_mutex_t                 _memMutex;
    std::map< BackMem*, ArrayBuf* > _backArray; // MT
    std::map< ArrayBuf*, BackMem* > _arrayBack; // MT
    std::map< ArrayBuf*, size_t >   _arrayCount; // MT

    // keep track of compute device memory by trace hash code
    std::map< uint64_t, std::map< uint32_t, ArrayBuf* > > _liveArray; // MT
    std::map< uint64_t, std::map< uint32_t, ArrayBuf* > > _deadArray;
    std::map< uint64_t, std::map< const AstVariable*, ArrayBuf* > > _splitArray;

    using MemBase::allocBackMem;
    using MemBase::allocFrontMem;

public:
    enum AccessMode { READ = 0, WRITE = 1, READWRITE = 2 };

private:
    ArrayBuf::AccessMode convertMode(const AccessMode) const;

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t PREC,
                             const size_t vecLen,
                             const size_t W,
                             const size_t H,
                             const size_t slots);

    template <typename SCALAR>
    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t vecLen,
                             const size_t W,
                             const size_t H,
                             const size_t slots,
                             SCALAR* ptr)
    {
        ArrayBuf* arrayBuf = new ArrayBuf( *_oclDevice,
                                           convertMode(mode),
                                           vecLen,
                                           W,
                                           H,
                                           slots,
                                           ptr );

        if (arrayBuf->bufOk())
        {
            return arrayBuf;
        }
        else
        {
            delete arrayBuf;
            return NULL;
        }
    }

    bool createArrayInternal(const uint32_t varNum,
                             const AccessMode mode,
                             const size_t PREC,
                             const size_t vecLen,
                             const size_t W,
                             const size_t H,
                             const size_t slots,
                             VectorTrace& vt,
                             const bool initBackMem,
                             const float val32,
                             const double val64,
                             const int32_t val32i,
                             const uint32_t val32u);

    bool createArrayInternal(const AstVariable* varPtr,
                             const AccessMode mode,
                             const size_t PREC,
                             const size_t vecLen,
                             const size_t W,
                             const size_t H,
                             const size_t slots,
                             VectorTrace& vt,
                             const bool initBackMem,
                             const float val32,
                             const double val64,
                             const int32_t val32i,
                             const uint32_t val32u);

public:
    // translator/JIT uses OpenCL
    MemTrans(const int deviceNum,  // deviceNum is OCLinit index
             OCLinit&,
             OCLdevice&);

    ~MemTrans(void);

    ////////////////////////////////////////
    // MemAlloc

    bool memalloc(VectorTrace& vt);
    void memfree(VectorTrace& vt, const bool freeMem);

    ////////////////////////////////////////
    // MemCallback

    void swizzle(BackMem*);
    void untrackMem(BackMem*);

    ////////////////////////////////////////
    // accessors

    int deviceNum(void) const;
    OCLdevice* computeDevice(void) const;
    ArrayBuf* arrayBuf(const uint32_t traceVar, VectorTrace&);
    ArrayBuf* arrayBuf(const AstVariable* splitVar, VectorTrace&);

    ////////////////////////////////////////
    // JIT only

    BackMem* unifyBackMem( const uint32_t varNum,
                           const uint32_t verNum,
                           const std::vector< FrontMem* >& frontMem );

    bool enqueueKernel(VectorTrace& vt,
                       KernelUnit& func,
                       const size_t globalWidth,
                       const size_t globalHeight,
                       const size_t localWidth,
                       const size_t localHeight);

    bool enqueueKernel(VectorTrace& vt,
                       KernelUnit& func,
                       const size_t globalWidth,
                       const size_t globalHeight = 0);

    ////////////////////////////////////////
    // create device memory

    bool createArraySendData(const uint32_t varNum,
                             const AccessMode mode,
                             const size_t vecLen,
                             VectorTrace& vt,
                             BackMem* backMem);

    bool createArraySendData(const AstVariable* varPtr,
                             const AccessMode mode,
                             const size_t vecLen,
                             VectorTrace& vt,
                             BackMem* backMem);

    template <typename VARIABLE>
    bool createArrayTemp(const VARIABLE variable,
                         const AccessMode mode,
                         const size_t PREC,
                         const size_t vecLen,
                         const size_t W,
                         const size_t H,
                         const size_t slots,
                         VectorTrace& vt)
    {
        return createArrayInternal(variable,
                                   mode,
                                   PREC,
                                   vecLen,
                                   W,
                                   H,
                                   slots,
                                   vt,
                                   false,
                                   0,
                                   0,
                                   0,
                                   0);
    }

    template <typename VARIABLE, typename SCALAR>
    bool createArrayTemp(const VARIABLE variable,
                         const AccessMode mode,
                         const size_t vecLen,
                         const size_t W,
                         const size_t H,
                         const size_t slots,
                         VectorTrace& vt,
                         const SCALAR value)
    {
        return createArrayInternal(variable,
                                   mode,
                                   PrecType::getCode<SCALAR>(),
                                   vecLen,
                                   W,
                                   H,
                                   slots,
                                   vt,
                                   false,
                                   value,
                                   value,
                                   value,
                                   value);
    }
};

}; // namespace chai_internal

#endif
