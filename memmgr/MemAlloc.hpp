// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_ALLOC_HPP_
#define _CHAI_MEM_ALLOC_HPP_

#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// scheduler allocates backing memory

class MemAlloc
{
public:
    // allocate backing memory
    bool memalloc(VectorTrace& vt);

    // release backing memory on failure
    void memfree(VectorTrace& vt, const bool freeMem);
};

}; // namespace chai_internal

#endif
