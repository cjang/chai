// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtLiteral.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declare literal

StmtLiteral::StmtLiteral(AstVariable* lhs,
                         AstLitdata* rhs)
    : _rhsLitdata(rhs),
      _rhsScalar(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // the literal assignment is code
    buoyancySink();
}

StmtLiteral::StmtLiteral(AstVariable* lhs,
                         AstScalar* rhs)
    : _rhsLitdata(NULL),
      _rhsScalar(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // the literal assignment is code
    buoyancySink();
}

bool StmtLiteral::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtLiteral::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstLitdata* StmtLiteral::litdataPtr(void) const
{
    return _rhsLitdata;
}

AstScalar* StmtLiteral::scalarPtr(void) const
{
    return _rhsScalar;
}

}; // namespace chai_internal
