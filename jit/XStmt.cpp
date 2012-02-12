// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmt.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// working AST statement

XStmt::XStmt(void)
    : _lhsVariable(NULL),
      _rhsVariable(),
      _underlyingVars(),
      _transposeTraceVars(),
      _transposeSplitVars(),
      _gatherTraceVars(),
      _gatherSplitVars(),
      _buoyancy(NEUTRAL),
      _surfaceBuoyancy(false) { }

XStmt::~XStmt(void) { }

void XStmt::lhsVariable(AstVariable* v)
{
    _lhsVariable = v;

    if (v->isTraceVariable())
    {
        // trace variable
        _traceLHS.insert(v->variable());
    }
    else
    {
        // split variable
        _splitLHS.insert(v);
    }
}

void XStmt::rhsVariable(AstVariable* v)
{
    _rhsVariable.insert(v);

    if (v->isTraceVariable())
    {
        // trace variable
        _traceRHS.insert(v->variable());
    }
    else
    {
        // split variable
        _splitRHS.insert(v);
    }
}

void XStmt::buoyancySurface(void)
{
    _buoyancy = RISE;
    _surfaceBuoyancy = true;
}

void XStmt::buoyancyRise(void)
{
    _buoyancy = RISE;
}

void XStmt::buoyancyNeutral(void)
{
    _buoyancy = NEUTRAL;
}

void XStmt::buoyancySink(void)
{
    _buoyancy = SINK;
}

void XStmt::setBuoyancy(const XStmt& other)
{
    _buoyancy = other._buoyancy;
}

int XStmt::getBuoyancy(void) const
{
    return _buoyancy;
}

bool XStmt::surfaceBuoyancy(void) const
{
    return _surfaceBuoyancy;
}

AstVariable* XStmt::lhsVariable(void) const
{
    return _lhsVariable;
}

const set< AstVariable* >& XStmt::rhsVariable(void) const
{
    return _rhsVariable;
}

const set< AstVariable* >& XStmt::underlyingVars(void) const
{
    return _underlyingVars;
}

void XStmt::underlyingVars(const set< AstVariable* >& underlyingVars)
{
    _underlyingVars = underlyingVars;
}

const set< uint32_t >& XStmt::transposeTraceVars(void) const
{
    return _transposeTraceVars;
}

const set< AstVariable* >& XStmt::transposeSplitVars(void) const
{
    return _transposeSplitVars;
}

void XStmt::transposeVars(const set< uint32_t >& traceNums,
                          const set< AstVariable* >& splitPtrs)
{
    _transposeTraceVars = traceNums;
    _transposeSplitVars = splitPtrs;
}

const set< uint32_t >& XStmt::gatherTraceVars(void) const
{
    return _gatherTraceVars;
}

const set< AstVariable* >& XStmt::gatherSplitVars(void) const
{
    return _gatherSplitVars;
}

void XStmt::gatherVars(const set< uint32_t >& traceNums,
                       const set< AstVariable* >& splitPtrs)
{
    _gatherTraceVars = traceNums;
    _gatherSplitVars = splitPtrs;
}

bool XStmt::trackLHS(void) const
{
    return true;
}

bool XStmt::swappable(const XStmt& other) const
{
    // Any collision with a LHS variable implies order dependency.
    // There is no order dependency from common RHS variables, though.

    // trace variables //

    // this LHS with other LHS and RHS
    for (set< uint32_t >::const_iterator
         it = _traceLHS.begin();
         it != _traceLHS.end();
         it++)
    {
        if (other._traceLHS.count(*it) || other._traceRHS.count(*it))
        {
            return false;
        }
    }

    // this RHS with other LHS
    for (set< uint32_t >::const_iterator
         it = _traceRHS.begin();
         it != _traceRHS.end();
         it++)
    {
        if (other._traceLHS.count(*it))
        {
            return false;
        }
    }

    // split variables //

    // this LHS with other LHS and RHS
    for (set< AstVariable* >::const_iterator
         it = _splitLHS.begin();
         it != _splitLHS.end();
         it++)
    {
        if (other._splitLHS.count(*it) || other._splitRHS.count(*it))
        {
            return false;
        }
    }

    // this RHS with other LHS
    for (set< AstVariable* >::const_iterator
         it = _splitRHS.begin();
         it != _splitRHS.end();
         it++)
    {
        if (other._splitLHS.count(*it))
        {
            return false;
        }
    }

    // ok to swap
    return true;
}

}; // namespace chai_internal
