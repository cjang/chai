// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_SEND_DATA_HPP_
#define _CHAI_STMT_SEND_DATA_HPP_

#include "AstArrayMem.hpp"
#include "AstMakedata.hpp"
#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// send array memory to compute device
// case 1. AstArrayMem - intermediate variable values
// case 2. AstMakedata - new array data

class StmtSendData : public Stmt
{
    // RHS
    AstArrayMem* _rhsArrayMem;
    AstMakedata* _rhsMakedata;

public:
    StmtSendData(AstVariable* lhs, AstArrayMem* rhs);
    StmtSendData(AstVariable* lhs, AstMakedata* rhs);

    bool trackLHS(void) const;

    bool randomness(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstArrayMem* arrayMemPtr(void) const;
    AstMakedata* makedataPtr(void) const;
};

}; // namespace chai_internal

#endif
