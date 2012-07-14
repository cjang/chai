// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VECTOR_TRACE_HPP_
#define _CHAI_VECTOR_TRACE_HPP_

#include <map>
#include <pthread.h>
#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include "ArrayBuf.hpp"
#include "BackMem.hpp"
#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "FrontMem.hpp"
#include "SingleTrace.hpp"
#include "VectorNut.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// parallel traces with same hash code

class VectorTrace
{
    // hash codes for a single trace
    uint64_t                _hashCode;

    std::set< uint32_t >    _liveVariables;

    // left hand side
    std::vector< uint32_t > _lhsVariable;
    std::vector< uint32_t > _lhsVersion;

    // right hand side
    std::vector< Stak<BC> > _rhsBytecode;

    size_t                  _numTraces;

    // statement pointers to array memory boxes
    std::map< size_t, std::vector< FrontMem* > > _frontMem;
    std::map< size_t, uint32_t >                 _frontVar;
    std::map< uint32_t, size_t >                 _varFront;

    // deferred allocation of memory for the boxes
    // statement index -> back memory object
    std::map< size_t, BackMem* >                 _backMem;

    // trace variable number -> statement index
    std::map< uint32_t, size_t >                 _traceToStmt;

    // front memory offsets into back memory for memalloc'ed trace variables
    std::map< uint32_t,
              std::pair< size_t,
                         std::vector< size_t >
                       > >                       _memallocFrontIndex;

    // writeback to variable nuts
    std::map< uint32_t, VectorNut* > _vectorNuts;

    // need all hash codes when repeatedly scheduling traces without finalizing
    std::vector< uint64_t > _hashCodeHistory;

    // restuck from one compute device to another
    bool _stickyMovement;

    // constrain vector length choice by JIT
    std::map< uint32_t, int > _forceVecLength;

public:
    VectorTrace(const std::map< pthread_t, SingleTrace* >& traceSet);

    ~VectorTrace(void);

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;
    size_t traceLength(void) const;

    const std::vector< uint64_t >& hashCodeHistory(void) const;

    bool stickyMovement(void) const;

    // statement index is relative due to checkpoints
    uint32_t getVariable(const size_t stmtIndex) const;
    uint32_t getVersion(const size_t stmtIndex) const;
    Stak<BC>& getRHS(const size_t stmtIndex);
    const Stak<BC>& getRHS(const size_t stmtIndex) const;

    // array memory boxes
    std::map< size_t, std::vector< FrontMem* > >& frontMem(void);
    const std::map< size_t, std::vector< FrontMem* > >& frontMem(void) const;
    uint32_t frontVar(const size_t stmtIndex); // absolute statement index
    size_t varFront(const uint32_t traceNum);

    // deferred allocation of array memory
    std::map< size_t, BackMem* >& backMem(void);
    const std::map< size_t, BackMem* >& backMem(void) const;

    // front memory offsets into back memory for memalloc'ed trace variables
    void memallocFront(const uint32_t variable,
                       const size_t frontSize,
                       const std::vector< size_t >& frontIndex);
    size_t memallocFrontSize(const uint32_t variable) const;
    std::vector< size_t > memallocFrontIndex(const uint32_t variable) const;

    // for storing stream values with variables
    std::map< uint32_t, VectorNut* >& vectorNuts(void);
    const std::map< uint32_t, VectorNut* >& vectorNuts(void) const;

    // live variables
    const std::set< uint32_t >& liveVariables(void) const;

    // constrain vector length choice by JIT
    const std::map< uint32_t, int >& forceVecLength(void) const;
};

}; // namespace chai_internal

#endif
