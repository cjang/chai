// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_FRONT_MEM_HPP_
#define _CHAI_FRONT_MEM_HPP_

#include <cstddef>

#include "BackMem.hpp"
#include "RefCnt.hpp"

namespace chai_internal {

////////////////////////////////////////
// reference counted array memory

class FrontMem : public RefObj
{
    const size_t _W;
    const size_t _H;

    // precision
    enum Type { FLOAT, DOUBLE };
    const Type   _ptrType;

    // managed pointer into the backing memory
    void*        _ptrMem;

    // backing memory
    BackMem*     _backObj;

    // copy to _ptrMem
    void*        _dataPtr;

public:
    FrontMem(const size_t W, const size_t H, float* dataPtr);
    FrontMem(const size_t W, const size_t H, double* dataPtr);

    ~FrontMem(void);

    size_t sizeElems(void) const;
    size_t sizeBytes(void) const;

    size_t W(void) const;
    size_t H(void) const;

    bool isFloat(void) const;
    bool isDouble(void) const;

    float* floatPtr(void) const;
    double* doublePtr(void) const;
    void* ptrMem(void) const;
    void* dataPtr(void) const;

    BackMem* getBackMem(void) const;
    void setBackMem(void* ptr, BackMem* obj);
    void clearBackMem(void);
};

}; // namespace chai_internal

#endif
