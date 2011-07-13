// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VISIT_BC_STMT_HPP_
#define _CHAI_VISIT_BC_STMT_HPP_

namespace chai_internal {

class BCStmtCompound;
class BCStmtRepeat;
class BCStmtSingle;

////////////////////////////////////////
// visitor for bytecode statements

struct VisitBCStmt
{
    virtual ~VisitBCStmt(void);

    virtual void visit(BCStmtCompound&) = 0;
    virtual void visit(BCStmtRepeat&) = 0;
    virtual void visit(BCStmtSingle&) = 0;
};

}; // namespace chai_internal

#endif
