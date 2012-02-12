// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VISIT_X_STMT_HPP_
#define _CHAI_VISIT_X_STMT_HPP_

namespace chai_internal {

class XStmtBarrier;
class XStmtCompound;
class XStmtCreateData;
class XStmtExtension;
class XStmtExtensionAuto;
class XStmtIdSpace;
class XStmtIndex;
class XStmtLiteral;
class XStmtMatmul;
class XStmtMatmulAuto;
class XStmtReadData;
class XStmtReduce;
class XStmtRepeat;
class XStmtRNGrand;
class XStmtRNGseed;
class XStmtSendData;
class XStmtSingle;

////////////////////////////////////////
// visitor for AST statements

struct VisitXStmt
{
    virtual ~VisitXStmt(void);

    virtual void visit(XStmtBarrier&) = 0;
    virtual void visit(XStmtCompound&) = 0;
    virtual void visit(XStmtCreateData&) = 0;
    virtual void visit(XStmtExtension&) = 0;
    virtual void visit(XStmtExtensionAuto&) = 0;
    virtual void visit(XStmtIdSpace&) = 0;
    virtual void visit(XStmtIndex&) = 0;
    virtual void visit(XStmtLiteral&) = 0;
    virtual void visit(XStmtMatmul&) = 0;
    virtual void visit(XStmtMatmulAuto&) = 0;
    virtual void visit(XStmtReadData&) = 0;
    virtual void visit(XStmtReduce&) = 0;
    virtual void visit(XStmtRepeat&) = 0;
    virtual void visit(XStmtRNGrand&) = 0;
    virtual void visit(XStmtRNGseed&) = 0;
    virtual void visit(XStmtSendData&) = 0;
    virtual void visit(XStmtSingle&) = 0;
};

}; // namespace chai_internal

#endif
