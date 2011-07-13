// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_READ_DATA_HPP_
#define _CHAI_X_STMT_READ_DATA_HPP_

#include "AstReadout.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// read array memory from compute device

class XStmtReadData : public XStmt
{
    // RHS
    AstReadout*  _rhsReadout;

public:
    XStmtReadData(AstVariable* lhs, AstReadout* rhs);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstReadout* readoutPtr(void) const;
};

}; // namespace chai_internal

#endif
