// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtLiteral.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declare literal

XStmtLiteral::XStmtLiteral(AstVariable* lhs,
                           AstLitdata* rhs)
    : _rhsLitdata(rhs),
      _rhsScalar(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // the literal assignment is code
    buoyancySink();
}

XStmtLiteral::XStmtLiteral(AstVariable* lhs,
                           AstScalar* rhs)
    : _rhsLitdata(NULL),
      _rhsScalar(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // the literal assignment is code
    buoyancySink();
}

bool XStmtLiteral::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtLiteral::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstLitdata* XStmtLiteral::litdataPtr(void) const
{
    return _rhsLitdata;
}

AstScalar* XStmtLiteral::scalarPtr(void) const
{
    return _rhsScalar;
}

}; // namespace chai_internal
