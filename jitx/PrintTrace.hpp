// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_PRINT_TRACE_HPP_
#define _CHAI_PRINT_TRACE_HPP_

#include <ostream>

#include "BasePrinter.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// print converted trace

class PrintTrace : public VisitStmt,
                   public BasePrinter
{
    const bool _descendIdSpace;

    // repeat for-loop variable names
    size_t     _repeatIndex;

public:
    PrintTrace(std::ostream&,
               const bool descendIdSpace = true);

    void visit(StmtBarrier&);
    void visit(StmtCompound&);
    void visit(StmtCreateData&);
    void visit(StmtExtension&);
    void visit(StmtExtensionAuto&);
    void visit(StmtIdSpace&);
    void visit(StmtIndex&);
    void visit(StmtLiteral&);
    void visit(StmtMatmul&);
    void visit(StmtMatmulAuto&);
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtRNGrand&);
    void visit(StmtRNGseed&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);

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
