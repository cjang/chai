// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_INDEX_HPP_
#define _CHAI_X_STMT_INDEX_HPP_

#include "AstIdxdata.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// index array

class XStmtIndex : public XStmt
{
    // RHS
    AstIdxdata*  _rhsIdxdata;

public:
    XStmtIndex(AstVariable* lhs, AstIdxdata* rhs);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstIdxdata* idxdataPtr(void) const;
};

}; // namespace chai_internal

#endif
