// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_COMPOUND_HPP_
#define _CHAI_X_STMT_COMPOUND_HPP_

#include <vector>

#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// compound AST statement

class XStmtCompound : public XStmt
{
    std::vector< XStmt* > _stmts;

public:
    XStmtCompound(void);
    ~XStmtCompound(void);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    std::vector< XStmt* >& stuffInside(void);
    const std::vector< XStmt* >& stuffInside(void) const;
};

}; // namespace chai_internal

#endif
