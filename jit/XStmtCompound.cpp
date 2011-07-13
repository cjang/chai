// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "XStmtCompound.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// compound AST statement

XStmtCompound::XStmtCompound(void)
{
    // loop bodies don't rise or sink
    buoyancyNeutral();
}

XStmtCompound::~XStmtCompound(void)
{
    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

bool XStmtCompound::swappable(const XStmt& other) const
{
    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        if (! (*it)->swappable(other))
            return false;
    }

    // ok to swap
    return true;
}

void XStmtCompound::accept(VisitXStmt& v)
{
    v.visit(*this);
}

vector< XStmt* >& XStmtCompound::stuffInside(void)
{
    return _stmts;
}

const vector< XStmt* >& XStmtCompound::stuffInside(void) const
{
    return _stmts;
}

}; // namespace chai_internal
