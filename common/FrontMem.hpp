// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_FRONT_MEM_HPP_
#define _CHAI_FRONT_MEM_HPP_

#include <cstddef>
#include <stdint.h>
#include <vector>

#include "BackMem.hpp"
#include "chai/RefCnt.hpp"

namespace chai_internal {

////////////////////////////////////////
// reference counted array memory

class FrontMem : public RefObj
{
    // trace variable number
    const uint32_t _variable;

    // dimensions
    const size_t _W;
    const size_t _H;
    const size_t _slots;

    // precision
    const size_t _precision;

    // managed pointer into the backing memory
    void*        _ptrMem;

    // backing memory
    BackMem*     _backObj;

    // copy to _ptrMem
    void*        _dataPtr;

    // vector array data
    std::vector< FrontMem* > _slotMem;

public:
    // used by interpreter
    FrontMem(const size_t W, const size_t H, float* dataPtr);
    FrontMem(const size_t W, const size_t H, double* dataPtr);
    FrontMem(const size_t W, const size_t H, int32_t* dataPtr);
    FrontMem(const size_t W, const size_t H, uint32_t* dataPtr);

    // used by read_scalar, read1, read2
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const size_t precision);

    // used by read_scalar, read1, read2 (vector array data)
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const size_t precision,
             const size_t slots);

    // used by make1, make2
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             float* dataPtr);

    // used by make1, make2
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             double* dataPtr);

    // used by make1, make2
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             int32_t* dataPtr);

    // used by make1, make2
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             uint32_t* dataPtr);

    // used by make1, make2 (vector array data)
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const std::vector< float* >& dataPtr);

    // used by make1, make2 (vector array data)
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const std::vector< double* >& dataPtr);

    // used by make1, make2 (vector array data)
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const std::vector< int32_t* >& dataPtr);

    // used by make1, make2 (vector array data)
    FrontMem(const uint32_t variable,
             const size_t W,
             const size_t H,
             const std::vector< uint32_t* >& dataPtr);

    // used by inline OpenCL
    FrontMem(const uint32_t variable,
             void* ptr);

    ~FrontMem(void);

    size_t sizeBytes(void) const;

    uint32_t variable(void) const;

    size_t W(void) const;
    size_t H(void) const;
    size_t slots(void) const;

    size_t precision(void) const;

    void swizzle(const size_t uniqueKey);

    float* floatPtr(void) const;
    double* doublePtr(void) const;
    int32_t* intPtr(void) const;
    uint32_t* uintPtr(void) const;
    void* ptrMem(void) const;
    void* ptrMem(const size_t precision) const;
    void* dataPtr(void) const;

    const std::vector< FrontMem* >& slotMem(void) const;

    BackMem* getBackMem(void) const;
    void setBackMem(void* ptr, BackMem* obj);
    void clearBackMem(void);
};

}; // namespace chai_internal

#endif
