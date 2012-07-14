// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <string.h>

#include "Logger.hpp"
#include "MemTrans.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

BackMem* MemTrans::unifyBackMem(const uint32_t varNum,
                                const uint32_t verNum,
                                const vector< FrontMem* >& frontMem)
{
    const FrontMem* proto = frontMem.front();

    // special case bailout if inline OpenCL
    if (0 == proto->W() || 0 == proto->H())
        return NULL;

    BackMem* backMem = NULL;

    if (1 == frontMem.size())
    {
        backMem = proto->getBackMem();
    }

    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(proto->prec(),
                                                 proto->W());
    const size_t frontSize
        = proto->H() * frontWidth * PrecType::sizeOf(proto->prec());

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

            switch (proto->prec())
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
                    = proto->W() * PrecType::sizeOf(proto->prec());

                const size_t padRowSize
                    = frontWidth * PrecType::sizeOf(proto->prec());

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

bool MemTrans::enqueueKernel(VectorTrace& vt,
                             KernelUnit& func,
                             const size_t globalWidth,
                             const size_t globalHeight,
                             const size_t localWidth,
                             const size_t localHeight)
{
    // check cache
    OCLkernel* kernelObj = _cache->factoryLookup(func);
    if (! kernelObj)
    {
        // failed, no kernel available
        return false;
    }

    *kernelObj << OCLWorkIndex(globalWidth, localWidth);
    if (0 != globalHeight && 0 != localHeight)
    {
        *kernelObj << OCLWorkIndex(globalHeight, localHeight);
    }

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "index space width=" << globalWidth << "/" << localWidth;
    if (0 != globalHeight)
    {
        ss << " height=" << globalHeight << "/" << localHeight;
    }
    LOGGER(ss.str())
#endif

    // set kernel arguments
    for (vector< Variable* >::const_iterator
         it = func.arguments().begin(); it != func.arguments().end(); it++)
    {
        const AstVariable* astVar = func.splitVar(*it);
        const uint32_t     varNum = func.traceVar(*it);
        LocalMemory*       locMem = dynamic_cast< LocalMemory* >(*it);
        ScalarArgument*    sclArg = dynamic_cast< ScalarArgument* >(*it);

        if (astVar)
        {
            arrayBuf(astVar, vt)->argumentToKernel(*kernelObj);
        }
        else if (-1 != varNum)
        {
            arrayBuf(varNum, vt)->argumentToKernel(*kernelObj);
        }
        else if (locMem)
        {
            const size_t length = locMem->length();

            switch (locMem->prec())
            {
                case (PrecType::UInt32) :
                    *kernelObj << OCLLocalbuf<uint32_t>(length);
                    break;

                case (PrecType::Int32) :
                    *kernelObj << OCLLocalbuf<int32_t>(length);
                    break;

                case (PrecType::Float) :
                    *kernelObj << OCLLocalbuf<float>(length);
                    break;

                case (PrecType::Double) :
                    *kernelObj << OCLLocalbuf<double>(length);
                    break;
            }
        }
        else if (sclArg)
        {
            switch (sclArg->prec())
            {
                case (PrecType::UInt32) :
                    *kernelObj << sclArg->scalarUInt32();
                    break;

                case (PrecType::Int32) :
                    *kernelObj << sclArg->scalarInt32();
                    break;

                case (PrecType::Float) :
                    *kernelObj << sclArg->scalarFloat();
                    break;

                case (PrecType::Double) :
                    *kernelObj << sclArg->scalarDouble();
                    break;
            }
        }
    }

    // run kernel, note the flush triggers previous memory transfers too
    *_oclDevice << *kernelObj << FLUSH;

    // success
    return true;
}

bool MemTrans::enqueueKernel(VectorTrace& vt,
                             KernelUnit& func,
                             const size_t globalWidth,
                             const size_t globalHeight)
{
    // optimal work group size heuristic
    const size_t wgSize = OCLhacks::singleton().workGroupSize(_deviceNum);

    // index space width
    size_t localWidth = wgSize;
    while (true)
    {
        if (0 == globalWidth % localWidth)
        {
            // this work group size fits
            break;
        }

        if (0 == localWidth % 2)
        {
            // try half size in width
            localWidth = localWidth / 2;
        }
        else
        {
            // failed to find a work group size larger than default
            localWidth = 1;
            break;
        }
    }

    // index space height
    size_t localHeight = wgSize / localWidth;
    if (0 != globalHeight)
    {
        while (true)
        {
            if (0 == globalHeight % localHeight)
            {
                // this work group size fits
                break;
            }

            if (0 == localHeight % 2)
            {
                // try half size in height
                localHeight = localHeight / 2;
            }
            else
            {
                // failed to find a work group size larger than default
                localHeight = 1;
                break;
            }
        }
    }

    return enqueueKernel(vt,
                         func,
                         globalWidth,
                         globalHeight,
                         localWidth,
                         localHeight);
}

}; // namespace chai_internal
