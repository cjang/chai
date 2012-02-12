// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_PRINTER_HPP_
#define _CHAI_X_STMT_PRINTER_HPP_

#include <ostream>

#include "BasePrinter.hpp"
#include "VisitXStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// print converted trace

class XStmtPrinter : public VisitXStmt,
                     public BasePrinter
{
    const bool    _descendIdSpace;

    // repeat for-loop variable names
    size_t        _repeatIndex;

public:
    XStmtPrinter(std::ostream&,
                 const bool descendIdSpace = true);

    void visit(XStmtBarrier&);
    void visit(XStmtCompound&);
    void visit(XStmtCreateData&);
    void visit(XStmtExtension&);
    void visit(XStmtExtensionAuto&);
    void visit(XStmtIdSpace&);
    void visit(XStmtIndex&);
    void visit(XStmtLiteral&);
    void visit(XStmtMatmul&);
    void visit(XStmtMatmulAuto&);
    void visit(XStmtReadData&);
    void visit(XStmtReduce&);
    void visit(XStmtRepeat&);
    void visit(XStmtRNGrand&);
    void visit(XStmtRNGseed&);
    void visit(XStmtSendData&);
    void visit(XStmtSingle&);

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
