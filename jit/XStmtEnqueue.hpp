// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_ENQUEUE_HPP_
#define _CHAI_X_STMT_ENQUEUE_HPP_

#include <ostream>

#include "MemManager.hpp"
#include "VectorTrace.hpp"
#include "VisitXStmt.hpp"
#include "XStmtPrinter.hpp"

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace

class XStmtEnqueue : public VisitXStmt
{
    VectorTrace&  _vt;
    MemManager&   _memMgr;
    XStmtPrinter* _printer;
    std::ostream* _os;

    bool _failure;

public:
    XStmtEnqueue(VectorTrace&, MemManager&);
    XStmtEnqueue(VectorTrace&, MemManager&, std::ostream&);
    ~XStmtEnqueue(void);

    bool isOk(void) const;

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
};

}; // namespace chai_internal

#endif
