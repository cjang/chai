// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "MemalignSTL.hpp"
#include "MemBase.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

BackMem* MemBase::allocBackMem(const size_t PREC,
                               const size_t W,
                               const size_t H,
                               const size_t slots)
{
    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(PREC, W);
    const size_t frontSize = H * frontWidth * PrecType::sizeOf(PREC);

    void* b = NULL;

    if (0 == posix_memalign(&b,
                            MEMORY_ALIGNMENT,
                            frontSize * slots))
    {
        // JIT memory buffers are padded so the interpreter must match
        // initialize memory to zero
        memset(b, 0, frontSize * slots);

        BackMem* backMem = NULL;

        switch (PREC)
        {
            case (PrecType::UInt32) :
                backMem = new BackMem(W,
                                      H,
                                      slots,
                                      slots, // front count
                                      static_cast<uint32_t*>(b),
                                      this);
                break;

            case (PrecType::Int32) :
                backMem = new BackMem(W,
                                      H,
                                      slots,
                                      slots, // front count
                                      static_cast<int32_t*>(b),
                                      this);
                break;

            case (PrecType::Float) :
                backMem = new BackMem(W,
                                      H,
                                      slots,
                                      slots, // front count
                                      static_cast<float*>(b),
                                      this);
                break;

            case (PrecType::Double) :
                backMem = new BackMem(W,
                                      H,
                                      slots,
                                      slots, // front count
                                      static_cast<double*>(b),
                                      this);
                break;
        }

        // front memory owns the backing memory
        backMem->release();

        return backMem;
    }
    else
    {
        return NULL;
    }
}

FrontMem* MemBase::allocFrontMem(const size_t PREC,
                                   const size_t W,
                                   const size_t H,
                                   BackMem* backMem,
                                   const size_t slotIndex) const
{
    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(PREC, W);
    const size_t frontSize = H * frontWidth * PrecType::sizeOf(PREC);

    FrontMem* frontMem = NULL;

    switch (PREC)
    {
        case (PrecType::UInt32) :
            frontMem = new FrontMem(W, H, static_cast<uint32_t*>(NULL));
            break;

        case (PrecType::Int32) :
            frontMem = new FrontMem(W, H, static_cast<int32_t*>(NULL));
            break;

        case (PrecType::Float) :
            frontMem = new FrontMem(W, H, static_cast<float*>(NULL));
            break;

        case (PrecType::Double) :
            frontMem = new FrontMem(W, H, static_cast<double*>(NULL));
            break;
    }

    frontMem->setBackMem(
        static_cast< char* >(backMem->ptrMem()) + slotIndex * frontSize,
        backMem);

    return frontMem;
}

}; // namespace chai_internal
