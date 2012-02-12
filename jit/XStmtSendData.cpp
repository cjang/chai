// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtSendData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// send array memory to compute device
// case 1. AstArrayMem - intermediate variable values
// case 2. AstMakedata - new array data

XStmtSendData::XStmtSendData(AstVariable* lhs,
                             AstArrayMem* rhs,
                             const bool convert)
    : _rhsArrayMem(rhs),
      _rhsMakedata(NULL),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory assignments rise to the top
    buoyancyRise();
}

XStmtSendData::XStmtSendData(AstVariable* lhs,
                             AstMakedata* rhs,
                             const bool convert)
    : _rhsArrayMem(NULL),
      _rhsMakedata(rhs),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory assignments rise to the top
    buoyancyRise();
}

bool XStmtSendData::trackLHS(void) const
{
    return false;
}

bool XStmtSendData::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtSendData::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstArrayMem* XStmtSendData::arrayMemPtr(void) const
{
    return _rhsArrayMem;
}

AstMakedata* XStmtSendData::makedataPtr(void) const
{
    return _rhsMakedata;
}

bool XStmtSendData::isConvert(void) const
{
    return _isConvert;
}

}; // namespace chai_internal
