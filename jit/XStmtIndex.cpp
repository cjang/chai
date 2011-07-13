// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtIndex.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declare literal

XStmtIndex::XStmtIndex(AstVariable* lhs,
                       AstIdxdata* rhs)
    : _rhsIdxdata(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    buoyancySink();
}

bool XStmtIndex::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtIndex::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstIdxdata* XStmtIndex::idxdataPtr(void) const
{
    return _rhsIdxdata;
}

}; // namespace chai_internal
