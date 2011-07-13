// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_LITERAL_HPP_
#define _CHAI_X_STMT_LITERAL_HPP_

#include "AstLitdata.hpp"
#include "AstScalar.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// declare literal

class XStmtLiteral : public XStmt
{
    // RHS
    AstLitdata*  _rhsLitdata;
    AstScalar*   _rhsScalar;

public:
    XStmtLiteral(AstVariable* lhs, AstLitdata* rhs);
    XStmtLiteral(AstVariable* lhs, AstScalar* rhs);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstLitdata* litdataPtr(void) const;
    AstScalar* scalarPtr(void) const;
};

}; // namespace chai_internal

#endif
