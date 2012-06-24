// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include "Logger.hpp"
#include "MemManager.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

bool MemManager::enqueueKernel(VectorTrace& vt,
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
         it = func.arguments().begin();
         it != func.arguments().end();
         it++)
    {
        const AstVariable* astVar = func.splitVar(*it);
        const uint32_t     varNum = func.traceVar(*it);
        LocalMemory*       locMem = dynamic_cast< LocalMemory* >(*it);
        ScalarArgument*    sclArg = dynamic_cast< ScalarArgument* >(*it);

        if (astVar)
        {
            getArrayBuf(astVar, vt)->argumentToKernel(*kernelObj);
        }
        else if (-1 != varNum)
        {
            getArrayBuf(varNum, vt)->argumentToKernel(*kernelObj);
        }
        else if (locMem)
        {
            const size_t length = locMem->length();

            switch (locMem->precision())
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
            switch (sclArg->precision())
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

bool MemManager::enqueueKernel(VectorTrace& vt,
                               KernelUnit& func,
                               const size_t globalWidth,
                               const size_t globalHeight)
{
    // optimal work group size heuristic
    const size_t wgSize = OCLhacks::singleton().workGroupSize(_deviceIndex);

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
