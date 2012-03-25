// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MAKE_BC_STMT_HPP_
#define _CHAI_MAKE_BC_STMT_HPP_

#include <map>
#include <stack>
#include <vector>

#include "BaseAst.hpp"
#include "DispatchTrans.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create AST RHS statements from
// vectorized bytecode statement trace

class MakeBCStmt : public VisitBCStmt
{
    DispatchTrans& _opDisp;

    std::stack< BaseAst* > _outStack;

    // keep track of lexical scope of variables
    std::map< uint32_t, std::vector< BCStmtSingle* > > _varScope;

public:
    MakeBCStmt(DispatchTrans&);

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);
};

}; // namespace chai_internal

#endif
