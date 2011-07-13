// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_SEND_DATA_HPP_
#define _CHAI_X_STMT_SEND_DATA_HPP_

#include "AstArrayMem.hpp"
#include "AstMakedata.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// send array memory to compute device
// case 1. AstArrayMem - intermediate variable values
// case 2. AstMakedata - new array data

class XStmtSendData : public XStmt
{
    // RHS
    AstArrayMem* _rhsArrayMem;
    AstMakedata* _rhsMakedata;

    // if assignment requires type conversion 
    const bool   _isConvert;

public:
    XStmtSendData(AstVariable* lhs, AstArrayMem* rhs, const bool convert);
    XStmtSendData(AstVariable* lhs, AstMakedata* rhs, const bool convert);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstArrayMem* arrayMemPtr(void) const;
    AstMakedata* makedataPtr(void) const;
    bool isConvert(void) const;
};

}; // namespace chai_internal

#endif
