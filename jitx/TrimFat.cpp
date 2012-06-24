// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "StmtBarrier.hpp"
#include "StmtCompound.hpp"
#include "StmtCreateData.hpp"
#include "StmtExtension.hpp"
#include "StmtExtensionAuto.hpp"
#include "StmtGatherAuto.hpp"
#include "StmtIdSpace.hpp"
#include "StmtIndex.hpp"
#include "StmtLiteral.hpp"
#include "StmtMatmul.hpp"
#include "StmtMatmulAuto.hpp"
#include "StmtOpenCL.hpp"
#include "StmtReadData.hpp"
#include "StmtReduce.hpp"
#include "StmtRepeat.hpp"
#include "StmtSendData.hpp"
#include "StmtSingle.hpp"
#include "TrimFat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// remove useless variables between
// kernels that have become private
// registers

TrimFat::TrimFat(const set< uint32_t >& traceUseRegs,
                 const set< uint32_t >& traceUseRegsWriteBack)
    : _traceUseRegs(traceUseRegs),
      _traceUseRegsWriteBack(traceUseRegsWriteBack),
      _traceCreateVars() { }

void TrimFat::edit(StmtIdSpace& xid)
{
    // iterate over and traverse within kernel statements
    for (vector< Stmt* >::const_iterator
         it = xid.stuffInside().begin();
         it != xid.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }

    vector< Stmt* > newStmts;

    // remove statements
    for (vector< Stmt* >::const_iterator
         it = xid.stuffInside().begin();
         it != xid.stuffInside().end();
         it++)
    {
        Stmt* stmtPtr = *it;

        bool keepStmt = true;

        if (_traceCreateVars.count(stmtPtr))
        {
            const uint32_t varNum = _traceCreateVars[stmtPtr];

            const bool trimStmt = _traceUseRegs.count(varNum) &&
                                  ! _traceUseRegsWriteBack.count(varNum);

            if (trimStmt)
            {
                keepStmt = false;
            }
        }

        if (keepStmt)
        {
            newStmts.push_back(stmtPtr);
        }
        else
        {
            delete stmtPtr;
        }
    }

    xid.replaceStuff(newStmts);
}

////////////////////////////////////////
// VisitStmt

void TrimFat::visit(StmtBarrier& s) { }

void TrimFat::visit(StmtCompound& s) { }

void TrimFat::visit(StmtCreateData& s)
{
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();

        _traceCreateVars[ &s ] = varNum;
    }
}

void TrimFat::visit(StmtExtension& s) { }

void TrimFat::visit(StmtExtensionAuto& s) { }

void TrimFat::visit(StmtGatherAuto& s) { }

void TrimFat::visit(StmtIdSpace& s) { }

void TrimFat::visit(StmtIndex& s) { }

void TrimFat::visit(StmtLiteral& s) { }

void TrimFat::visit(StmtMatmul& s) { }

void TrimFat::visit(StmtMatmulAuto& s) { }

void TrimFat::visit(StmtOpenCL& s) { }

void TrimFat::visit(StmtReadData& s) { }

void TrimFat::visit(StmtReduce& s) { }

void TrimFat::visit(StmtRepeat& s) { }

void TrimFat::visit(StmtSendData& s) { }

void TrimFat::visit(StmtSingle& s) { }

}; // namespace chai_internal
