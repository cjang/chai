// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_GATHER_HPP_
#define _CHAI_X_STMT_GATHER_HPP_

#include "AstGather.hpp"
#include "AstVariable.hpp"
#include "VisitAst.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// gather

class XStmtGather : public XStmt,
                    public VisitAst
{
    // RHS
    AstGather*   _rhsGather;

    // if assignment requires type conversion
    const bool   _isConvert;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    XStmtGather(AstVariable* lhs, AstGather* rhs, const bool convert);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstGather* gatherPtr(void) const;
    bool isConvert(void) const;

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
