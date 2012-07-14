// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRIM_FAT_HPP_
#define _CHAI_TRIM_FAT_HPP_

#include <map>
#include <set>
#include <stdint.h>

#include "Stmt.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// remove useless variables between
// kernels that have become private
// registers

class TrimFat : public VisitStmt
{
    // variables that are elided as private registers
    const std::set< uint32_t >           _traceUseRegs;
    const std::set< uint32_t >           _traceUseRegsWriteBack;

    // create data statement variables
    std::map< Stmt*, uint32_t >           _traceCreateVars;

public:
    TrimFat(const std::set< uint32_t >& traceUseRegs,
            const std::set< uint32_t >& traceUseRegsWriteBack);

    void edit(StmtIdSpace&);

    void visit(StmtBarrier&);
    void visit(StmtCompound&);
    void visit(StmtCreateData&);
    void visit(StmtExtension&);
    void visit(StmtExtensionAuto&);
    void visit(StmtGatherAuto&);
    void visit(StmtIdSpace&);
    void visit(StmtIndex&);
    void visit(StmtLiteral&);
    void visit(StmtMatmul&);
    void visit(StmtMatmulAuto&);
    void visit(StmtOpenCL&);
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);
};

}; // namespace chai_internal

#endif
