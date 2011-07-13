// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ARRAY_CLIENT_HPP_
#define _CHAI_ARRAY_CLIENT_HPP_

#include <cstddef>
#include <pthread.h>
#include <stdint.h>
#include <vector>

#include "BC.hpp"
#include "ClientTrace.hpp"
#include "FrontMem.hpp"
#include "SingleNut.hpp"
#include "Stak.hpp"

namespace chai_internal {

////////////////////////////////////////
// singleton per thread
// array variable interaction with scheduler

class ArrayClient
{
    const pthread_t _ptself;
    size_t          _variableCnt;
    ClientTrace     _trace;

public:
    ArrayClient(const pthread_t ptself);

    size_t variable(void);

    void constructor(const uint32_t variable,
                     SingleNut* variableNut);
    void destructor(const uint32_t variable);

    Stak<BC>& assignment(const uint32_t variable,
                         const uint32_t version);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       float* dataPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       double* dataPtr);

    void* frontMem(FrontMem*);

    void schedule(void);
};

}; // namespace chai_internal

#endif
