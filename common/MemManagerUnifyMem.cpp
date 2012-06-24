// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <string.h>

#include "Logger.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

bool MemManager::checkSameDataAcrossTraces(
                     const vector< FrontMem* >& frontMem )
{
    if (frontMem.empty())
        return false;

    // Check for same data across all traces:
    // 1. all front objects have the same back object
    // 2. front and back objects are the same dimensions

    const FrontMem* proto = frontMem.front();

    // special case bailout if inline OpenCL
    if (0 == proto->W() || 0 == proto->H())
        return false;

    set< BackMem* > backPtrs;
    for (vector< FrontMem* >::const_iterator
         it = frontMem.begin();
         it != frontMem.end();
         it++)
    {
        backPtrs.insert((*it)->getBackMem());
    }

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = PrecType::padWidth(proto->precision(),
                                                 proto->W());

    return 1 == backPtrs.size() &&
           1 == proto->getBackMem()->packing() &&
           frontWidth == proto->getBackMem()->W() &&
           proto->H() == proto->getBackMem()->H();
}

BackMem* MemManager::unifyBackMem(const uint32_t variable,
                                  const uint32_t version,
                                  vector< FrontMem* >& frontMem)
{
#ifdef __LOGGING_ENABLED__
    for (size_t i = 0; i < frontMem.size(); i++)
    {
        stringstream ss;
        ss << "frontMem[" << i << "]=" << frontMem[i];
        LOGGER(ss.str())
    }
#endif

    const FrontMem* proto = frontMem.front();

    // special case bailout if inline OpenCL
    if (0 == proto->W() || 0 == proto->H())
        return NULL;

    BackMem* backMem = NULL;

    if (checkSameDataAcrossTraces(frontMem))
    {
        backMem = proto->getBackMem();

#ifdef __LOGGING_ENABLED__
        if (NULL == backMem->ptrMem())
            LOGGER("back mem has null ptr")
#endif
    }

    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(proto->precision(),
                                                 proto->W());
    const size_t frontSize
        = proto->H() * frontWidth * PrecType::sizeOf(proto->precision());

    void* b = NULL;

    // allocate backing memory
    if (NULL == backMem)
    {
        if (0 == posix_memalign(&b,
                                MEMORY_ALIGNMENT,
                                frontSize * frontMem.size()))
        {
            // allows summing oversized buffer in read_scalar()
            // initialize memory to zero
            memset(b, 0, frontSize * frontMem.size());

            switch (proto->precision())
            {
                case (PrecType::UInt32) :
                    backMem = new BackMem(frontWidth,
                                          proto->H(),
                                          frontMem.size(),
                                          frontMem.size(), // front count
                                          static_cast<uint32_t*>(b),
                                          this);
                    break;

                case (PrecType::Int32) :
                    backMem = new BackMem(frontWidth,
                                          proto->H(),
                                          frontMem.size(),
                                          frontMem.size(), // front count
                                          static_cast<int32_t*>(b),
                                          this);
                    break;

                case (PrecType::Float) :
                    backMem = new BackMem(frontWidth,
                                          proto->H(),
                                          frontMem.size(),
                                          frontMem.size(), // front count
                                          static_cast<float*>(b),
                                          this);
                    break;

                case (PrecType::Double) :
                    backMem = new BackMem(frontWidth,
                                          proto->H(),
                                          frontMem.size(),
                                          frontMem.size(), // front count
                                          static_cast<double*>(b),
                                          this);
                    break;
            }
        }
        else
        {
            // posix_memalign failed
            return NULL;
        }
    }

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "variable=" << variable
       << " version=" << version
       << " backMem=" << backMem;
    LOGGER(ss.str())
#endif

    for (size_t i = 0; i < frontMem.size(); i++)
    {
        // only need to copy if new backing memory allocated
        if (NULL != b)
        {
            // Padding makes the front memory boxes larger than the
            // deferred data. Row-major memory layout is also an issue.
            if (frontWidth > proto->W())
            {
                const size_t dataRowSize
                    = proto->W() * PrecType::sizeOf(proto->precision());

                const size_t padRowSize
                    = frontWidth * PrecType::sizeOf(proto->precision());

                for (size_t j = 0; j < proto->H(); j++)
                {
                    memcpy(static_cast< char* >(b)
                               + frontSize * i
                               + padRowSize * j,
                           static_cast< char* >(frontMem[i]->ptrMem())
                               + dataRowSize * j,
                           dataRowSize);

                    memset(static_cast< char* >(b)
                               + frontSize * i
                               + padRowSize * j
                               + dataRowSize,
                           0,
                           padRowSize - dataRowSize);
                }
            }
            else
            {
                // no padding, simple memory copy
                memcpy(static_cast< char* >(b) + frontSize * i,
                       frontMem[i]->ptrMem(),
                       frontSize);
            }
        }

        if (frontMem[i]->getBackMem() != backMem)
        {
            frontMem[i]->clearBackMem();
            frontMem[i]->setBackMem(
                             static_cast< char* >(backMem->ptrMem())
                                 + frontSize * i,
                             backMem );
        }
    }

    // check if re-used old backing memory or allocated new object
    if (NULL != b)
    {
        // front memory owns the backing memory
        backMem->release();
    }

    return backMem;
}

}; // namespace chai_internal
