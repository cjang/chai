// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "StmtCompound.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// compound AST statement

StmtCompound::StmtCompound(void)
{
    // loop bodies don't rise or sink
    buoyancyNeutral();
}

StmtCompound::~StmtCompound(void)
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

bool StmtCompound::swappable(const Stmt& other) const
{
    for (vector< Stmt* >::const_iterator
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

void StmtCompound::accept(VisitStmt& v)
{
    v.visit(*this);
}

vector< Stmt* >& StmtCompound::stuffInside(void)
{
    return _stmts;
}

const vector< Stmt* >& StmtCompound::stuffInside(void) const
{
    return _stmts;
}

}; // namespace chai_internal
