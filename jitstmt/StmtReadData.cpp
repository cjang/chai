// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtReadData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read array memory from compute device

StmtReadData::StmtReadData(AstVariable* lhs,
                           AstReadout* rhs)
    : _rhsReadout(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // readout is always last
    buoyancySink();
}

bool StmtReadData::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtReadData::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstReadout* StmtReadData::readoutPtr(void) const
{
    return _rhsReadout;
}

}; // namespace chai_internal
