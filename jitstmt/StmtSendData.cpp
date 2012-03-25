// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtSendData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// send array memory to compute device
// case 1. AstArrayMem - intermediate variable values
// case 2. AstMakedata - new array data

StmtSendData::StmtSendData(AstVariable* lhs,
                           AstArrayMem* rhs)
    : _rhsArrayMem(rhs),
      _rhsMakedata(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory assignments rise to the top
    buoyancyRise();
}

StmtSendData::StmtSendData(AstVariable* lhs,
                           AstMakedata* rhs)
    : _rhsArrayMem(NULL),
      _rhsMakedata(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory assignments rise to the top
    buoyancyRise();
}

bool StmtSendData::trackLHS(void) const
{
    return false;
}

bool StmtSendData::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtSendData::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstArrayMem* StmtSendData::arrayMemPtr(void) const
{
    return _rhsArrayMem;
}

AstMakedata* StmtSendData::makedataPtr(void) const
{
    return _rhsMakedata;
}

}; // namespace chai_internal
