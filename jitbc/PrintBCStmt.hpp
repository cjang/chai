// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_PRINT_BC_STMT_HPP_
#define _CHAI_PRINT_BC_STMT_HPP_

#include <ostream>

#include "BasePrinter.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// print bytecode statement trace

class PrintBCStmt : public VisitBCStmt,
                    public BasePrinter
{
    // repeat for-loop variable names
    size_t _repeatIndex;

public:
    PrintBCStmt(std::ostream&);

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);

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
    void visit(AstReadout&);
    void visit(AstRNGnormal&);
    void visit(AstRNGuniform&);
    void visit(AstScalar&);
    void visit(AstTranspose&);
    void visit(AstVariable&);
};

}; // namespace chai_internal

#endif
