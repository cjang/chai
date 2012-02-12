// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BACK_MEM_HPP_
#define _CHAI_BACK_MEM_HPP_

#include <cstddef>
#include <set>

#include "chai/RefCnt.hpp"

namespace chai_internal {

class MemManager;

////////////////////////////////////////
// reference counted array memory

class BackMem : public RefObj
{
    const size_t _W;
    const size_t _H;
    const size_t _packing;

    // number of FrontMem objects referencing this backing memory
    const size_t _frontCount;

    // precision
    enum Type { FLOAT, DOUBLE };
    const Type   _ptrType;

    // managed pointer
    void*        _ptrMem;

    // data moves between compute devices
    MemManager*  _memMgr;

    // avoid redundant swizzling from front memory
    std::set< size_t > _swizzleSet;

public:
    BackMem(const size_t W,
            const size_t H,
            const size_t packing,
            const size_t frontCount,
            float* ptr,
            MemManager* mm);

    BackMem(const size_t W,
            const size_t H,
            const size_t packing,
            const size_t frontCount,
            double* ptr,
            MemManager* mm);

    ~BackMem(void);

    size_t sizeElems(void) const;

    size_t W(void) const;
    size_t H(void) const;
    size_t packing(void) const;

    size_t frontCount(void) const;

    bool isFloat(void) const;
    bool isDouble(void) const;
    size_t precision(void) const;

    void* ptrMem(void) const;

    void swizzle(void);                   // used by translator
    void swizzle(const size_t uniqueKey); // used by interpreter
};

}; // namespace chai_internal

#endif
