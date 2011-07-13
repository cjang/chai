// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_MAKER_HPP_
#define _CHAI_BC_STMT_MAKER_HPP_

#include <stack>
#include <vector>

#include "BaseAst.hpp"
#include "DispatchTrans.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create AST RHS statements from
// vectorized bytecode statement trace

class BCStmtMaker : public VisitBCStmt
{
    DispatchTrans& _opDisp;

    std::stack< BaseAst* > _outStack;

public:
    BCStmtMaker(DispatchTrans&);

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);
};

}; // namespace chai_internal

#endif
