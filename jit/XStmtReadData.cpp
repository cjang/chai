// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtReadData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read array memory from compute device

XStmtReadData::XStmtReadData(AstVariable* lhs,
                             AstReadout* rhs)
    : _rhsReadout(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // readout is always last
    buoyancySink();
}

bool XStmtReadData::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtReadData::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstReadout* XStmtReadData::readoutPtr(void) const
{
    return _rhsReadout;
}

}; // namespace chai_internal
