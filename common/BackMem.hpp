// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BACK_MEM_HPP_
#define _CHAI_BACK_MEM_HPP_

#include <cstddef>
#include <set>
#include <stdint.h>

#include "chai/RefCnt.hpp"

namespace chai_internal {

class MemCallback;

////////////////////////////////////////
// reference counted array memory

class BackMem : public RefObj
{
    const size_t _W;
    const size_t _H;
    const size_t _slots;

    // number of FrontMem objects referencing this backing memory
    const size_t _frontCount;

    // precision
    const size_t _prec;

    // managed pointer
    void*        _ptrMem;

    // data moves between compute devices
    MemCallback* _memMgr;

    // avoid redundant swizzling from front memory
    std::set< size_t > _swizzleSet;

public:
    BackMem(const size_t W,
            const size_t H,
            const size_t slots,
            const size_t frontCount,
            float* ptr,
            MemCallback* mm);

    BackMem(const size_t W,
            const size_t H,
            const size_t slots,
            const size_t frontCount,
            double* ptr,
            MemCallback* mm);

    BackMem(const size_t W,
            const size_t H,
            const size_t slots,
            const size_t frontCount,
            int32_t* ptr,
            MemCallback* mm);

    BackMem(const size_t W,
            const size_t H,
            const size_t slots,
            const size_t frontCount,
            uint32_t* ptr,
            MemCallback* mm);

    ~BackMem(void);

    size_t sizeElems(void) const;

    size_t W(void) const;
    size_t H(void) const;
    size_t slots(void) const;

    size_t frontCount(void) const;

    size_t prec(void) const;

    void* ptrMem(void) const;

    void swizzle(void);                   // used by translator
    void swizzle(const size_t uniqueKey); // used by interpreter
};

}; // namespace chai_internal

#endif
