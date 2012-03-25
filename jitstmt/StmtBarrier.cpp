// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtBarrier.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// thread barrier statement

bool StmtBarrier::checkMatch(const AstVariable* varPtr) const
{
    if (varPtr)
    {
        if (varPtr->isTraceVariable())
        {
            return _traceVars.count(varPtr->variable());
        }
        else
        {
            return _splitVars.count(varPtr);
        }
    }
    else
    {
        return false;
    }
}

StmtBarrier::StmtBarrier( const set< uint32_t >& traceVars,
                          const set< const AstVariable* >& splitVars )
    : _traceVars(traceVars),
      _splitVars(splitVars)
{
    // loop bodies don't rise or sink
    buoyancyNeutral();
}

StmtBarrier::~StmtBarrier(void) { }

bool StmtBarrier::containedIn(const set< uint32_t >& traceVars) const
{
    if (_splitVars.empty())
    {
        for (set< uint32_t >::const_iterator
             it = _traceVars.begin();
             it != _traceVars.end();
             it++)
        {
            if (0 == traceVars.count(*it))
                return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool StmtBarrier::containedIn(const set< uint32_t >& traceVars,
                              const set< const AstVariable* >& splitVars) const
{
    // contained inside trace set
    for (set< uint32_t >::const_iterator
         it = _traceVars.begin();
         it != _traceVars.end();
         it++)
    {
        if (0 == traceVars.count(*it))
            return false;
    }

    // contained inside split set
    for (set< const AstVariable* >::const_iterator
         it = _splitVars.begin();
         it != _splitVars.end();
         it++)
    {
        if (0 == splitVars.count(*it))
            return false;
    }

    return true;
}

const set< uint32_t >& StmtBarrier::traceVars(void) const
{
    return _traceVars;
}

bool StmtBarrier::swappable(const Stmt& other) const
{
    if (checkMatch(other.lhsVariable()))
        return false;

    for (set< AstVariable* >::const_iterator
         it = other.rhsVariable().begin();
         it != other.rhsVariable().end();
         it++)
    {
        if (checkMatch(*it))
            return false;
    }

    return true;
}

void StmtBarrier::accept(VisitStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
