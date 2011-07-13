// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "BCStmtCompound.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

BCStmtCompound::BCStmtCompound(void)
    : _hashStmtCount(0),
      _hashCode(0) { }

BCStmtCompound::~BCStmtCompound(void)
{
    for (vector< BCStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

uint64_t BCStmtCompound::hashCode(void)
{
    if (0 == _stmts.size())
        return 0;

    if (_hashStmtCount != _stmts.size())
    {
        vector< uint64_t > v;

        for (vector< BCStmt* >::const_iterator
             it = _stmts.begin();
             it != _stmts.end();
             it++)
        {
            v.push_back( (*it)->hashCode() );
        }

        _hashStmtCount = _stmts.size();
        _hashCode = TEA::hash(&v[0], v.size());
    }

    return _hashCode;
}

void BCStmtCompound::debug(ostream& os)
{
    os << "BEGIN Compound(" << hashCode() << ")" << endl;
    for (vector< BCStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        (*it)->debug(os);
    }
    os << "END Compound(" << hashCode() << ")" << endl;
}

const vector< BCStmt* >& BCStmtCompound::stuffInside(void) const
{
    return _stmts;
}

void BCStmtCompound::accept(VisitBCStmt& v)
{
    v.visit(*this);
}

void BCStmtCompound::push(BCStmt* s)
{
    _stmts.push_back(s);
}

}; // namespace chai_internal
