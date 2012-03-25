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
// remove variables that have become
// private registers
// kernels

class TrimFat : public VisitStmt
{
    // variables that are elided as private registers
    const std::set< uint32_t > _useRegs;
    const std::set< uint32_t > _useRegsWriteBack;

    // create data statement variables
    std::map< Stmt*, uint32_t > _createVars;

public:
    TrimFat(const std::set< uint32_t >& useRegs,
            const std::set< uint32_t >& useRegsWriteBack);

    void edit(StmtIdSpace&);

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
};

}; // namespace chai_internal

#endif
