// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_INTERP_HPP_
#define _CHAI_MEM_INTERP_HPP_

#include <cstddef>

#include "BackMem.hpp"
#include "FrontMem.hpp"
#include "MemAlloc.hpp"
#include "MemBase.hpp"
#include "MemCallback.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// memory manager for interpreter

class MemInterp : public MemBase
{
public:
    virtual ~MemInterp(void);

    ////////////////////////////////////////
    // MemAlloc

    bool memalloc(VectorTrace& vt);
    void memfree(VectorTrace& vt, const bool freeMem);

    ////////////////////////////////////////
    // MemCallback

    void swizzle(BackMem*);
    void untrackMem(BackMem*);

    ////////////////////////////////////////
    // interpreter only

    using MemBase::allocBackMem;
    using MemBase::allocFrontMem;
};

}; // namespace chai_internal

#endif
