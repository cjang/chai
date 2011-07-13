// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtIdSpace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

XStmtIdSpace::XStmtIdSpace(const XStmtTogether& together,
                           const VectorTrace& vt)
    : _stmts(together.stuffInside()),
      _allVariable(together.allVariable()),
      _lhsVariable(together.lhsVariable()),
      _rhsVariable(together.rhsVariable()),
      _indexW(together.indexW()),
      _indexH(together.indexH()),
      _numTraces(vt.numTraces())
{
    buoyancyNeutral();
}

XStmtIdSpace::~XStmtIdSpace(void)
{
    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

const set< AstVariable* >& XStmtIdSpace::allVariable(void) const
{
    return _allVariable;
}

const set< AstVariable* >& XStmtIdSpace::lhsVariable(void) const
{
    return _lhsVariable;
}

const set< AstVariable* >& XStmtIdSpace::rhsVariable(void) const
{
    return _rhsVariable;
}

size_t XStmtIdSpace::indexW(void) const
{
    return _indexW;
}

size_t XStmtIdSpace::indexH(void) const
{
    return _indexH;
}

size_t XStmtIdSpace::numTraces(void) const
{
    return _numTraces;
}

bool XStmtIdSpace::swappable(const XStmt& other) const
{
    return false;
}

void XStmtIdSpace::accept(VisitXStmt& v)
{
    v.visit(*this);
}

const vector< XStmt* >& XStmtIdSpace::stuffInside(void) const
{
    return _stmts;
}

}; // namespace chai_internal
