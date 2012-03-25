// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtRNGrand.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// random number generation

StmtRNGrand::StmtRNGrand(AstVariable* lhs,
                         AstRNGnormal* rhs)
    : _rhsRNGnormal(rhs),
      _rhsRNGuniform(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
}

StmtRNGrand::StmtRNGrand(AstVariable* lhs,
                         AstRNGuniform* rhs)
    : _rhsRNGnormal(NULL),
      _rhsRNGuniform(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
}

bool StmtRNGrand::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtRNGrand::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstRNGnormal* StmtRNGrand::normalPtr(void) const
{
    return _rhsRNGnormal;
}

AstRNGuniform* StmtRNGrand::uniformPtr(void) const
{
    return _rhsRNGuniform;
}

}; // namespace chai_internal
