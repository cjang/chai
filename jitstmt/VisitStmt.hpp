// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VISIT_STMT_HPP_
#define _CHAI_VISIT_STMT_HPP_

namespace chai_internal {

class StmtBarrier;
class StmtCompound;
class StmtCreateData;
class StmtExtension;
class StmtExtensionAuto;
class StmtGatherAuto;
class StmtIdSpace;
class StmtIndex;
class StmtLiteral;
class StmtMatmul;
class StmtMatmulAuto;
class StmtOpenCL;
class StmtReadData;
class StmtReduce;
class StmtRepeat;
class StmtSendData;
class StmtSingle;

////////////////////////////////////////
// visitor for AST statements

struct VisitStmt
{
    virtual ~VisitStmt(void);

    virtual void visit(StmtBarrier&) = 0;
    virtual void visit(StmtCompound&) = 0;
    virtual void visit(StmtCreateData&) = 0;
    virtual void visit(StmtExtension&) = 0;
    virtual void visit(StmtExtensionAuto&) = 0;
    virtual void visit(StmtGatherAuto&) = 0;
    virtual void visit(StmtIdSpace&) = 0;
    virtual void visit(StmtIndex&) = 0;
    virtual void visit(StmtLiteral&) = 0;
    virtual void visit(StmtMatmul&) = 0;
    virtual void visit(StmtMatmulAuto&) = 0;
    virtual void visit(StmtOpenCL&) = 0;
    virtual void visit(StmtReadData&) = 0;
    virtual void visit(StmtReduce&) = 0;
    virtual void visit(StmtRepeat&) = 0;
    virtual void visit(StmtSendData&) = 0;
    virtual void visit(StmtSingle&) = 0;
};

}; // namespace chai_internal

#endif
