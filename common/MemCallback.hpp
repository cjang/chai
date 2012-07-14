// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_CALLBACK_HPP_
#define _CHAI_MEM_CALLBACK_HPP_

#include "BackMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// callbacks from backing memory objects

class MemCallback
{
public:
    // data movement from compute device back to CPU
    virtual void swizzle(BackMem*) = 0;

    // backing memory does not need managing any more
    virtual void untrackMem(BackMem*) = 0;
};

}; // namespace chai_internal

#endif
