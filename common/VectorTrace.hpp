// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VECTOR_TRACE_HPP_
#define _CHAI_VECTOR_TRACE_HPP_

#include <map>
#include <set>
#include <stdint.h>
#include <vector>

#include "BackMem.hpp"
#include "BC.hpp"
#include "DeviceMem.hpp"
#include "FrontMem.hpp"
#include "SingleNut.hpp"
#include "SingleTrace.hpp"
#include "Stak.hpp"
#include "VectorNut.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// parallel traces with same hash code

class VectorTrace
{
    // hash codes for a single trace
    const uint64_t          _hashCode;

    std::set< uint32_t >    _liveVariables;

    // left hand side
    std::vector< uint32_t > _lhsVariable;
    std::vector< uint32_t > _lhsVersion;

    // right hand side
    std::vector< Stak<BC> > _rhsBytecode;

    const size_t            _numTraces;

    // statement pointers to array memory boxes
    std::map< size_t, std::vector< FrontMem* > > _frontMem;
    std::map< size_t, uint32_t >                 _frontVar;

    // deferred allocation of memory for the boxes
    std::map< size_t, BackMem* >                 _backMem;

    // compute device memory objects
    std::map< uint32_t, DeviceMem* >             _traceMem;
    std::map< AstVariable*, DeviceMem* >         _splitMem;

    // writeback to variable nuts
    std::map< uint32_t, VectorNut* >             _vectorNuts;

public:
    VectorTrace(const uint64_t hashCode,
                const std::set< SingleTrace* >& traceSet);

    ~VectorTrace(void);

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;
    size_t traceLength(void) const;

    // statement index is relative due to checkpoints
    uint32_t getVariable(const size_t stmtIndex) const;
    uint32_t getVersion(const size_t stmtIndex) const;
    Stak<BC>& getRHS(const size_t stmtIndex);
    const Stak<BC>& getRHS(const size_t stmtIndex) const;

    // array memory boxes
    std::map< size_t, std::vector< FrontMem* > >& frontMem(void);
    const std::map< size_t, std::vector< FrontMem* > >& frontMem(void) const;
    uint32_t frontVar(const size_t stmtIndex); // absolute statement index

    // deferred allocation of array memory
    std::map< size_t, BackMem* >& backMem(void);
    const std::map< size_t, BackMem* >& backMem(void) const;

    // compute device memory objects
    std::map< uint32_t, DeviceMem* >& traceMem(void);
    const std::map< uint32_t, DeviceMem* >& traceMem(void) const;
    std::map< AstVariable*, DeviceMem* >& splitMem(void);
    const std::map< AstVariable*, DeviceMem* >& splitMem(void) const;

    // for storing stream values with variables
    std::map< uint32_t, VectorNut* >& vectorNuts(void);
    const std::map< uint32_t, VectorNut* >& vectorNuts(void) const;

    // live variables
    const std::set< uint32_t >& liveVariables(void) const;
};

}; // namespace chai_internal

#endif
