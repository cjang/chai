// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CLIENT_TRACE_HPP_
#define _CHAI_CLIENT_TRACE_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>
#include <vector>

#include "chai/BC.hpp"
#include "chai/RefCnt.hpp"
#include "chai/Stak.hpp"
#include "FrontMem.hpp"
#include "SingleNut.hpp"

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

    // hash codes from single traces created from this client object
    std::vector< uint64_t >          _hashCodeHistory;

    // history of sticky continuation
    std::vector< size_t >            _stickyDeviceCode;

public:
    ClientTrace(void);

    size_t getScheduleCheckpoint(void);
    void setScheduleCheckpoint(void);

    void pushHashCode(const uint64_t);
    const std::vector< uint64_t >& hashCodeHistory(void) const;

    size_t stickyDevice(void) const;
    bool stickyDevice(const size_t deviceCode);
    void unstickyDevice(void);
    bool stickyMovement(void) const;

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
                       const size_t precision);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       const size_t precision,
                       const size_t slots);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       uint32_t* defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       int32_t* defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       float* defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       double* defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       const std::vector< uint32_t* >& defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       const std::vector< int32_t* >& defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       const std::vector< float* >& defPtr);

    FrontMem* memalloc(const uint32_t variable,
                       const size_t W,
                       const size_t H,
                       const std::vector< double* >& defPtr);

    size_t frontMem(FrontMem*);
};

}; // namespace chai_internal

#endif
