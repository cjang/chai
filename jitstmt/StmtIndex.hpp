// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_INDEX_HPP_
#define _CHAI_STMT_INDEX_HPP_

#include "AstIdxdata.hpp"
#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// index array

class StmtIndex : public Stmt
{
    // RHS
    AstIdxdata*  _rhsIdxdata;

public:
    StmtIndex(AstVariable* lhs, AstIdxdata* rhs);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstIdxdata* idxdataPtr(void) const;
};

}; // namespace chai_internal

#endif
