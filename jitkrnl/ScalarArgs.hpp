// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SCALAR_ARGS_HPP_
#define _CHAI_SCALAR_ARGS_HPP_

#include <vector>

#include "Stmt.hpp"
#include "VisitAst.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// scalar AST objects to kernel arguments

class ScalarArgs : public VisitAst,
                   public VisitStmt
{
    std::vector< AstScalar* > _scalarAST;

    void descendAst(BaseAst&);

public:
    ScalarArgs(void);

    void edit(StmtIdSpace&);

    void visit(AstAccum&);
    void visit(AstArrayMem&);
    void visit(AstCond&);
    void visit(AstConvert&);
    void visit(AstDotprod&);
    void visit(AstFun1&);
    void visit(AstFun2&);
    void visit(AstFun3&);
    void visit(AstGather&);
    void visit(AstIdxdata&);
    void visit(AstLitdata&);
    void visit(AstMakedata&);
    void visit(AstMatmulMM&);
    void visit(AstMatmulMV&);
    void visit(AstMatmulVM&);
    void visit(AstMatmulVV&);
    void visit(AstOpenCL&);
    void visit(AstReadout&);
    void visit(AstRNGnormal&);
    void visit(AstRNGuniform&);
    void visit(AstScalar&);
    void visit(AstTranspose&);
    void visit(AstVariable&);

    void visit(StmtBarrier&);
    void visit(StmtCompound&);
    void visit(StmtCreateData&);
    void visit(StmtGatherAuto&);
    void visit(StmtIdSpace&);
    void visit(StmtIndex&);
    void visit(StmtLiteral&);
    void visit(StmtMatmul&);
    void visit(StmtMatmulAuto&);
    void visit(StmtOpenCL&);
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);
};

}; // namespace chai_internal

#endif
