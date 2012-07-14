// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_LITERAL_HPP_
#define _CHAI_STMT_LITERAL_HPP_

#include "AstLitdata.hpp"
#include "AstScalar.hpp"
#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// declare literal

class StmtLiteral : public Stmt
{
    // RHS
    AstLitdata*  _rhsLitdata;
    AstScalar*   _rhsScalar;

public:
    StmtLiteral(AstVariable* lhs, AstLitdata* rhs);
    StmtLiteral(AstVariable* lhs, AstScalar* rhs);

    bool randomness(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstLitdata* litdataPtr(void) const;
    AstScalar* scalarPtr(void) const;
};

}; // namespace chai_internal

#endif
