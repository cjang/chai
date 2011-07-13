// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtRNGrand.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// random number generation

XStmtRNGrand::XStmtRNGrand(AstVariable* lhs,
                           AstRNGnormal* rhs)
    : _rhsRNGnormal(rhs),
      _rhsRNGuniform(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
}

XStmtRNGrand::XStmtRNGrand(AstVariable* lhs,
                           AstRNGuniform* rhs)
    : _rhsRNGnormal(NULL),
      _rhsRNGuniform(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
}

bool XStmtRNGrand::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtRNGrand::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstRNGnormal* XStmtRNGrand::normalPtr(void) const
{
    return _rhsRNGnormal;
}

AstRNGuniform* XStmtRNGrand::uniformPtr(void) const
{
    return _rhsRNGuniform;
}

}; // namespace chai_internal
