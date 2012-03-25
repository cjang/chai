// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtIndex.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declare literal

StmtIndex::StmtIndex(AstVariable* lhs,
                     AstIdxdata* rhs)
    : _rhsIdxdata(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    buoyancySink();
}

bool StmtIndex::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtIndex::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstIdxdata* StmtIndex::idxdataPtr(void) const
{
    return _rhsIdxdata;
}

}; // namespace chai_internal
