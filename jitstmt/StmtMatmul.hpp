// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_MATMUL_HPP_
#define _CHAI_STMT_MATMUL_HPP_

#include "VisitAst.hpp"
#include "StmtMatmulBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// matrix multiplication
// (all cases except outer product)

class StmtMatmul : public StmtMatmulBase,
                   public VisitAst
{
    // RHS
    AstMatmulMM* _rhsMatmulMM;
    AstMatmulMV* _rhsMatmulMV;
    AstMatmulVM* _rhsMatmulVM;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    StmtMatmul(AstVariable* lhs, AstMatmulMM* rhs);
    StmtMatmul(AstVariable* lhs, AstMatmulMV* rhs);
    StmtMatmul(AstVariable* lhs, AstMatmulVM* rhs);

    void accept(VisitStmt&);

    AstMatmulMM* matmulPtr(void) const;
    AstMatmulMV* matvecPtr(void) const;
    AstMatmulVM* vecmatPtr(void) const;

    void visit(AstAccum&);
    void visit(AstArrayMem&);
    void visit(AstCond&);
    void visit(AstConvert&);
    void visit(AstDotprod&);
    void visit(AstExtension&);
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
};

}; // namespace chai_internal

#endif
