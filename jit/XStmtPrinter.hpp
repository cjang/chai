// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_PRINTER_HPP_
#define _CHAI_X_STMT_PRINTER_HPP_

#include <ostream>

#include "BaseAst.hpp"
#include "VisitAst.hpp"
#include "VisitXStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// print converted trace

class XStmtPrinter : public VisitXStmt,
                     public VisitAst
{
    const bool    _descendIdSpace;

    std::ostream& _os;

    // indentation
    size_t        _indent;

    // repeat for-loop variable names
    size_t        _repeatIndex;

    // distinguish LHS from RHS during AST descent
    bool          _descendVar;

    std::ostream& indent(void);

    void descendAst(const size_t idx, BaseAst&);

public:
    XStmtPrinter(std::ostream&,
                 const bool descendIdSpace = true);

    void visit(XStmtCompound&);
    void visit(XStmtCreateData&);
    void visit(XStmtGather&);
    void visit(XStmtIdSpace&);
    void visit(XStmtIndex&);
    void visit(XStmtLiteral&);
    void visit(XStmtMatmul&);
    void visit(XStmtReadData&);
    void visit(XStmtReduce&);
    void visit(XStmtRepeat&);
    void visit(XStmtRNGrand&);
    void visit(XStmtRNGseed&);
    void visit(XStmtSendData&);
    void visit(XStmtSingle&);

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
