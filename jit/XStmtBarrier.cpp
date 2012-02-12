// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtBarrier.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// thread barrier statement

bool XStmtBarrier::checkMatch(AstVariable* varPtr) const
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

XStmtBarrier::XStmtBarrier( const set< uint32_t >& traceVars,
                            const set< AstVariable* >& splitVars )
    : _traceVars(traceVars),
      _splitVars(splitVars)
{
    // loop bodies don't rise or sink
    buoyancyNeutral();
}

XStmtBarrier::~XStmtBarrier(void) { }

bool XStmtBarrier::swappable(const XStmt& other) const
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

void XStmtBarrier::accept(VisitXStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
