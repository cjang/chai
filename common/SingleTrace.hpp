// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SINGLE_TRACE_HPP_
#define _CHAI_SINGLE_TRACE_HPP_

#include <cstddef>
#include <stdint.h>
#include <map>
#include <set>
#include <vector>

#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "ClientTrace.hpp"
#include "FrontMem.hpp"
#include "SingleNut.hpp"

namespace chai_internal {

class VectorTrace;

////////////////////////////////////////
// single execution trace in scheduler

class SingleTrace : public RefObj
{
    friend class VectorTrace;

    // array variables still in scope
    const std::set< uint32_t >       _liveVariables;

    // left hand side
    std::vector< uint32_t >          _lhsVariable;
    std::vector< uint32_t >          _lhsVersion;

    // right hand side
    std::vector< Stak<BC> >          _rhsBytecode;

    // hash code
    uint64_t                         _hashCode;

    // pointers by statement
    std::map< size_t, FrontMem* >    _frontMem;
    std::map< size_t, uint32_t >     _frontVar;

    // for storing stream values with variables
    std::map< uint32_t, SingleNut* > _variableNuts;

    // need back reference to origin client trace for sticky continuation
    ClientTrace&                     _origin;

    uint64_t computeHash(ClientTrace&);

public:
    SingleTrace(ClientTrace&);

    uint64_t hashCode(void) const;
    const std::vector< uint64_t >& hashCodeHistory(void) const;

    size_t stickyDevice(void) const;
    bool stickyDevice(const size_t deviceCode);
    void unstickyDevice(void);
    bool stickyMovement(void) const;
};

}; // namespace chai_internal

#endif
