// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_READ_DATA_HPP_
#define _CHAI_STMT_READ_DATA_HPP_

#include "AstReadout.hpp"
#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// read array memory from compute device

class StmtReadData : public Stmt
{
    // RHS
    AstReadout*  _rhsReadout;

public:
    StmtReadData(AstVariable* lhs, AstReadout* rhs);

    bool randomness(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstReadout* readoutPtr(void) const;
};

}; // namespace chai_internal

#endif
