// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_MANAGER_HPP_
#define _CHAI_MEM_MANAGER_HPP_

#include <cstddef>
#include <map>
#include <pthread.h>
#include <set>
#include <stdint.h>
#include <vector>

#include "ArrayBuf.hpp"
#include "BackMem.hpp"
#include "FrontMem.hpp"
#include "Function.hpp"
#include "KernelCache.hpp"
#include "OCLdevice.hpp"
#include "OCLinit.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// memory manager

class MemManager
{
public:
    // base device codes
    static const size_t CPU_INTERP = 100;
    static const size_t GPU_OPENCL = 1000;

    enum AccessMode { READ = 0, WRITE = 1, READWRITE = 2 };

private:
    const size_t _deviceCode;
    const size_t _deviceIndex;
    const bool   _deviceIsCPU;
    OCLinit*     _oclInit;
    OCLdevice*   _oclDevice;

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

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t precision,
                             const size_t vectorLength);

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t vectorLength,
                             float* ptr);

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t vectorLength,
                             double* ptr);

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t vectorLength,
                             int32_t* ptr);

    ArrayBuf* createArrayBuf(const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t vectorLength,
                             uint32_t* ptr);

    bool createArrayInternal(const uint32_t varNum,
                             const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t precision,
                             const size_t vectorLength,
                             VectorTrace& vt,
                             const bool initBackMem,
                             const float val32,
                             const double val64,
                             const int32_t val32i,
                             const uint32_t val32u);

    bool createArrayInternal(const AstVariable* varPtr,
                             const AccessMode mode,
                             const size_t packing,
                             const size_t W,
                             const size_t H,
                             const size_t precision,
                             const size_t vectorLength,
                             VectorTrace& vt,
                             const bool initBackMem,
                             const float val32,
                             const double val64,
                             const int32_t val32i,
                             const uint32_t val32u);

public:
    // interpreter does not have any OpenCL-ness
    MemManager(const size_t deviceCode);

    // translator/JIT uses OpenCL
    MemManager(const size_t deviceCode,
               const size_t deviceIndex, // from OCLinit
               OCLinit&,
               OCLdevice&);

    ~MemManager(void);

    size_t getDeviceCode(void) const;
    size_t getDeviceIndex(void) const;
    OCLdevice& getComputeDevice(void);

    // used by scheduler (allocate backing parent memory)
    bool memalloc(VectorTrace& vt);                    // before kernel
    void memfree(VectorTrace& vt, const bool freeMem); // after kernel

    // data movement from compute device back to CPU
    void swizzle(BackMem*);

    // backing memory does not need managing any more
    void untrackMem(BackMem*);

    // used by JIT dispatching
    static bool checkSameDataAcrossTraces(
                    const std::vector< FrontMem* >& frontMem );
    BackMem* unifyBackMem( const uint32_t variable,
                           const uint32_t version,
                           std::vector< FrontMem* >& frontMem );

    // direct access to array buffer objects needed for matmul set arguments
    ArrayBuf* getArrayBuf(const uint32_t traceVar, VectorTrace&);
    ArrayBuf* getArrayBuf(const AstVariable* splitVar, VectorTrace&);

    // create memory on compute device (trace variable)
    bool createArraySendData(const uint32_t varNum,
                             const AccessMode mode,
                             const size_t vectorLength,
                             VectorTrace& vt,
                             BackMem* backMem);
    bool createArrayTemp(const uint32_t varNum,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t precision,
                         const size_t vectorLength,
                         VectorTrace& vt);
    bool createArrayTemp(const uint32_t varNum,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const float val32);
    bool createArrayTemp(const uint32_t varNum,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const double val64);
    bool createArrayTemp(const uint32_t varNum,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const int32_t val32i);
    bool createArrayTemp(const uint32_t varNum,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const uint32_t val32u);

    // create memory on compute device (split variable)
    bool createArraySendData(const AstVariable* varPtr,
                             const AccessMode mode,
                             const size_t vectorLength,
                             VectorTrace& vt,
                             BackMem* backMem);
    bool createArrayTemp(const AstVariable* varPtr,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t precision,
                         const size_t vectorLength,
                         VectorTrace& vt);
    bool createArrayTemp(const AstVariable* varPtr,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const float val32);
    bool createArrayTemp(const AstVariable* varPtr,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const double val64);
    bool createArrayTemp(const AstVariable* varPtr,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const int32_t val32i);
    bool createArrayTemp(const AstVariable* varPtr,
                         const AccessMode mode,
                         const size_t packing,
                         const size_t W,
                         const size_t H,
                         const size_t vectorLength,
                         VectorTrace& vt,
                         const uint32_t val32u);

    // used by interpreter during operation dispatch
    BackMem* allocBackMem(const size_t W,
                          const size_t H,
                          const size_t precision,
                          const size_t vecSize);
    FrontMem* allocFrontMem(const size_t W,
                            const size_t H,
                            const size_t precision,
                            BackMem* parent,
                            const size_t vecIndex) const;

    // enqueue kernel
    bool enqueueKernel(VectorTrace& vt,
                       Function& func,
                       const size_t globalWidth,
                       const size_t globalHeight = 0);
};

}; // namespace chai_internal

#endif
