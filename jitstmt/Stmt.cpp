// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Stmt.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// working AST statement

Stmt::Stmt(void)
    : _lhsVariable(NULL),
      _rhsVariable(),
      _underlyingVars(),
      _scalarVecLength(false),
      _constructorLHS(false),
      _destructorLHS(false),
      _buoyancy(NEUTRAL),
      _surfaceBuoyancy(false) { }

Stmt::~Stmt(void) { }

void Stmt::lhsVariable(AstVariable* v)
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

void Stmt::rhsVariable(AstVariable* v)
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

void Stmt::buoyancySurface(void)
{
    _buoyancy = RISE;
    _surfaceBuoyancy = true;
}

void Stmt::buoyancyRise(void)
{
    _buoyancy = RISE;
}

void Stmt::buoyancyNeutral(void)
{
    _buoyancy = NEUTRAL;
}

void Stmt::buoyancySink(void)
{
    _buoyancy = SINK;
}

void Stmt::setBuoyancy(const Stmt& other)
{
    _buoyancy = other._buoyancy;
}

int Stmt::getBuoyancy(void) const
{
    return _buoyancy;
}

bool Stmt::surfaceBuoyancy(void) const
{
    return _surfaceBuoyancy;
}

bool Stmt::getConstructorLHS(void) const
{
    return _constructorLHS;
}

bool Stmt::getDestructorLHS(void) const
{
    return _destructorLHS;
}

void Stmt::setConstructorLHS(void)
{
    _constructorLHS = true;
}

void Stmt::setDestructorLHS(void)
{
    _destructorLHS = true;
}

AstVariable* Stmt::lhsVariable(void) const
{
    return _lhsVariable;
}

const set< AstVariable* >& Stmt::rhsVariable(void) const
{
    return _rhsVariable;
}

const set< AstVariable* >& Stmt::underlyingVars(void) const
{
    return _underlyingVars;
}

void Stmt::underlyingVars(const set< AstVariable* >& underlyingVars)
{
    _underlyingVars = underlyingVars;
}

bool Stmt::scalarVecLength(void) const
{
    return _scalarVecLength;
}

void Stmt::scalarVecLength(const bool a)
{
    _scalarVecLength = a;
}

bool Stmt::trackLHS(void) const
{
    return true;
}

bool Stmt::randomness(void) const
{
    return _lhsVariable
               ? _lhsVariable->randomness()
               : false;
}

bool Stmt::swappable(const Stmt& other) const
{
    // Any collision with a LHS variable implies order dependency.
    // There is no order dependency from common RHS variables, though.

    // trace variables //

    // this LHS with other LHS and RHS
    for (set< uint32_t >::const_iterator
         it = _traceLHS.begin(); it != _traceLHS.end(); it++)
    {
        if (other._traceLHS.count(*it) || other._traceRHS.count(*it))
        {
            return false;
        }
    }

    // this RHS with other LHS
    for (set< uint32_t >::const_iterator
         it = _traceRHS.begin(); it != _traceRHS.end(); it++)
    {
        if (other._traceLHS.count(*it))
        {
            return false;
        }
    }

    // split variables //

    // this LHS with other LHS and RHS
    for (set< AstVariable* >::const_iterator
         it = _splitLHS.begin(); it != _splitLHS.end(); it++)
    {
        if (other._splitLHS.count(*it) || other._splitRHS.count(*it))
        {
            return false;
        }
    }

    // this RHS with other LHS
    for (set< AstVariable* >::const_iterator
         it = _splitRHS.begin(); it != _splitRHS.end(); it++)
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
