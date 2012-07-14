// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEM_BASE_HPP_
#define _CHAI_MEM_BASE_HPP_

#include <cstddef>

#include "BackMem.hpp"
#include "FrontMem.hpp"
#include "MemAlloc.hpp"
#include "MemCallback.hpp"

namespace chai_internal {

class MemBase : public MemAlloc,
                public MemCallback
{
public:
    BackMem* allocBackMem(const size_t PREC,
                          const size_t W,
                          const size_t H,
                          const size_t slots);

    FrontMem* allocFrontMem(const size_t PREC,
                            const size_t W,
                            const size_t H,
                            BackMem* parent,
                            const size_t slotIndex) const;
};

}; // namespace chai_internal

#endif
