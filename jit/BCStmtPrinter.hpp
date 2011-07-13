// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_PRINTER_HPP_
#define _CHAI_BC_STMT_PRINTER_HPP_

#include <ostream>

#include "BaseAst.hpp"
#include "VisitAst.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// print bytecode statement trace

class BCStmtPrinter : public VisitBCStmt,
                      public VisitAst
{
    std::ostream& _os;

    // indentation
    size_t        _indent;

    // repeat for-loop variable names
    size_t        _repeatIndex;

    // control AST variable LHS/RHS
    bool          _descendVar;

    std::ostream& indent(void);

    void descendAst(const size_t idx, BaseAst&);

public:
    BCStmtPrinter(std::ostream&);

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);

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
