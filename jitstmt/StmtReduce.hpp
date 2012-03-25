// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_REDUCE_HPP_
#define _CHAI_STMT_REDUCE_HPP_

#include "VisitAst.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// reduction to scalar

class StmtReduce : public Stmt,
                   public VisitAst
{
    // RHS
    AstAccum*    _rhsAccum;
    AstDotprod*  _rhsDotprod;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    StmtReduce(AstVariable* lhs, AstAccum* rhs);
    StmtReduce(AstVariable* lhs, AstDotprod* rhs);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstAccum* accumPtr(void) const;
    AstDotprod* dotprodPtr(void) const;

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
    void visit(AstReadout&);
    void visit(AstRNGnormal&);
    void visit(AstRNGuniform&);
    void visit(AstScalar&);
    void visit(AstTranspose&);
    void visit(AstVariable&);
};

}; // namespace chai_internal

#endif
