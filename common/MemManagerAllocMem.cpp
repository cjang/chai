// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <string.h>

#include "Logger.hpp"
#include "MemalignSTL.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// used by interpreter

BackMem* MemManager::allocBackMem(const size_t W,
                                  const size_t H,
                                  const size_t precision,
                                  const size_t vecSize)
{
    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(precision, W);
    const size_t frontSize = H * frontWidth * PrecType::sizeOf(precision);

    void* b = NULL;

    if (0 == posix_memalign(&b,
                            MEMORY_ALIGNMENT,
                            frontSize * vecSize))
    {
        // JIT memory buffers are padded so the interpreter must match
        // initialize memory to zero
        memset(b, 0, frontSize * vecSize);

        BackMem* backMem = NULL;

        switch (precision)
        {
            case (PrecType::UInt32) :
                backMem = new BackMem(W,
                                      H,
                                      vecSize,
                                      vecSize, // front count
                                      static_cast<uint32_t*>(b),
                                      this);
                break;

            case (PrecType::Int32) :
                backMem = new BackMem(W,
                                      H,
                                      vecSize,
                                      vecSize, // front count
                                      static_cast<int32_t*>(b),
                                      this);
                break;

            case (PrecType::Float) :
                backMem = new BackMem(W,
                                      H,
                                      vecSize,
                                      vecSize, // front count
                                      static_cast<float*>(b),
                                      this);
                break;

            case (PrecType::Double) :
                backMem = new BackMem(W,
                                      H,
                                      vecSize,
                                      vecSize, // front count
                                      static_cast<double*>(b),
                                      this);
                break;
        }

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << "backMem=" << backMem
           << " W=" << W
           << " H=" << H
           << " vecSize=" << vecSize
           << " precision=" << precision;
        LOGGER(ss.str())
#endif

        // front memory owns the backing memory
        backMem->release();

        return backMem;
    }
    else
    {
        return NULL;
    }
}

FrontMem* MemManager::allocFrontMem(const size_t W,
                                    const size_t H,
                                    const size_t precision,
                                    BackMem* backMem,
                                    const size_t vecIndex) const
{
    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(precision, W);
    const size_t frontSize = H * frontWidth * PrecType::sizeOf(precision);

    FrontMem* frontMem = NULL;

    switch (precision)
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

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "frontMem=" << frontMem;
    LOGGER(ss.str())
#endif

    frontMem->setBackMem(
        static_cast< char* >(backMem->ptrMem()) + vecIndex * frontSize,
        backMem);

    return frontMem;
}

}; // namespace chai_internal
