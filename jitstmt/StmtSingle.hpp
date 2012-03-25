// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_SINGLE_HPP_
#define _CHAI_STMT_SINGLE_HPP_

#include <set>
#include <vector>

#include "VisitAst.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// single AST statement

class StmtSingle : public Stmt,
                   public VisitAst
{
    // special case handling of scalar to scalar assignment
    bool _scalarToScalar;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    StmtSingle(AstVariable* lhs);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    size_t W(void) const;
    size_t H(void) const;
    size_t precision(void) const;

    void scalarToScalar(const bool);
    bool scalarToScalar(void) const;

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
