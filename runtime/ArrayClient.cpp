// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "ArrayClient.hpp"
#include "Scheduler.hpp"

using namespace std;

namespace chai_internal {

ArrayClient::ArrayClient(const pthread_t ptself)
    : _ptself(ptself),
      _variableCnt(0) { }

size_t ArrayClient::variable(void)
{
    return _variableCnt++;
}

void ArrayClient::constructor(const uint32_t variable,
                              SingleNut* variableNut)
{
    _trace.constructor(variable, variableNut);
}

void ArrayClient::destructor(const uint32_t variable)
{
    _trace.destructor(variable);

    if (_trace.final())
    {
        _variableCnt = 0;
        _trace.clear();
    }
}

Stak<BC>& ArrayClient::assignment(const uint32_t variable,
                                  const uint32_t version)
{
    return _trace.assignment(variable, version);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const size_t precision)
{
    return _trace.memalloc(variable, W, H, precision);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const size_t precision,
                                const size_t slots)
{
    return _trace.memalloc(variable, W, H, precision, slots);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                uint32_t* dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                int32_t* dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                float* dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                double* dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< uint32_t* >& dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< int32_t* >& dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< float* >& dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

FrontMem* ArrayClient::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< double* >& dataPtr)
{
    return _trace.memalloc(variable, W, H, dataPtr);
}

void* ArrayClient::frontMem(FrontMem* m)
{
    return reinterpret_cast<void*>(_trace.frontMem(m));
}

void ArrayClient::schedule(void)
{
    Scheduler::singleton().wait(_ptself, _trace);
}

}; // namespace chai_internal
