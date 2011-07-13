// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CLIENT_TRACE_HPP_
#define _CHAI_CLIENT_TRACE_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>
#include <vector>

#include "BC.hpp"
#include "FrontMem.hpp"
#include "RefCnt.hpp"
#include "SingleNut.hpp"
#include "Stak.hpp"

namespace chai_internal {

class SingleTrace;

////////////////////////////////////////
// array client execution trace

class ClientTrace
{
    friend class SingleTrace;

    // keep track of array variables alive and dead
    std::set< uint32_t >             _constructors;
    std::set< uint32_t >             _destructors;
    std::set< uint32_t >             _liveVariables;

    // left hand side
    std::vector< uint32_t >          _lhsVariable;
    std::vector< uint32_t >          _lhsVersion;

    // right hand side
    std::vector< Stak<BC> >          _rhsBytecode;

    // array memory and nuts
    RefSet                           _refs;

    // allocated array memory objects
    std::map< size_t, FrontMem* >    _frontMem;

    // for storing values with variables
    std::map< uint32_t, SingleNut* > _variableNuts;

    // evaluate statements from checkpoint to the end
    size_t                           _scheduleCheckpoint;

public:
    ClientTrace(void);

    size_t getScheduleCheckpoint(void);
    void setScheduleCheckpoint(void);

    // start the trace over completely from nothing
    void clear(void);

    // have all variables passed out of scope?
    bool final(void) const;

    void constructor(const uint32_t variable, SingleNut* variableNut);
    void destructor(const uint32_t variable);

    Stak<BC>& assignment(const uint32_t variable,
                         const uint32_t version);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       float* defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       double* defPtr);

    size_t frontMem(FrontMem*);
};

}; // namespace chai_internal

#endif
