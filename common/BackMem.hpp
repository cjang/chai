// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BACK_MEM_HPP_
#define _CHAI_BACK_MEM_HPP_

#include <cstddef>

#include "RefCnt.hpp"

namespace chai_internal {

class MemManager;

////////////////////////////////////////
// reference counted array memory

class BackMem : public RefObj
{
    const size_t _W;
    const size_t _H;

    // precision
    enum Type { FLOAT, DOUBLE };
    const Type   _ptrType;

    // managed pointer
    void*        _ptrMem;

    // data moves between compute devices
    MemManager*  _memMgr;

public:
    BackMem(const size_t W, const size_t H, float* ptr, MemManager* mm);
    BackMem(const size_t W, const size_t H, double* ptr, MemManager* mm);
    ~BackMem(void);

    size_t sizeElems(void) const;
    size_t sizeBytes(void) const;

    size_t W(void) const;
    size_t H(void) const;

    bool isFloat(void) const;
    bool isDouble(void) const;

    void* ptrMem(void) const;

    void swizzle(void);
};

}; // namespace chai_internal

#endif
