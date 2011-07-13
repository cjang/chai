// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SINGLE_TRACE_HPP_
#define _CHAI_SINGLE_TRACE_HPP_

#include <cstddef>
#include <stdint.h>
#include <map>
#include <set>
#include <vector>

#include "BC.hpp"
#include "ClientTrace.hpp"
#include "FrontMem.hpp"
#include "SingleNut.hpp"
#include "Stak.hpp"

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

    uint64_t computeHash(ClientTrace&);

public:
    SingleTrace(ClientTrace&);

    uint64_t hashCode(void) const;
};

}; // namespace chai_internal

#endif
