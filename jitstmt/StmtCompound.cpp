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
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        delete *it;
    }
}

bool StmtCompound::randomness(void) const
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        if ((*it)->randomness())
            return true;
    }

    return false;
}

bool StmtCompound::swappable(const Stmt& other) const
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
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

set< AstVariable* > StmtCompound::lhsInside(void) const
{
    set< AstVariable* > s;

    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        s.insert( (*it)->lhsVariable() );
    }

    return s;
}

set< AstVariable* > StmtCompound::rhsInside(void) const
{
    set< AstVariable* > s;

    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        s.insert( (*it)->rhsVariable().begin(),
                  (*it)->rhsVariable().end() );
    }

    return s;
}

}; // namespace chai_internal
