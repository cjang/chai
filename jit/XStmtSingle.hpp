// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_SINGLE_HPP_
#define _CHAI_X_STMT_SINGLE_HPP_

#include <set>
#include <vector>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "VisitAst.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// single AST statement

class XStmtSingle : public XStmt,
                    public VisitAst
{
    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    XStmtSingle(AstVariable* lhs);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    size_t W(void) const;
    size_t H(void) const;
    size_t precision(void) const;

    void visit(AstAccum&);
    void visit(AstArrayMem&);
    void visit(AstBinop&);
    void visit(AstCond&);
    void visit(AstConvert&);
    void visit(AstDotprod&);
    void visit(AstGather&);
    void visit(AstIdxdata&);
    void visit(AstIsomorph&);
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
    void visit(AstVariable&);
};

}; // namespace chai_internal

#endif
