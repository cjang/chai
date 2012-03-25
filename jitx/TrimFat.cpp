// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "StmtBarrier.hpp"
#include "StmtCompound.hpp"
#include "StmtCreateData.hpp"
#include "StmtExtension.hpp"
#include "StmtExtensionAuto.hpp"
#include "StmtIdSpace.hpp"
#include "StmtIndex.hpp"
#include "StmtLiteral.hpp"
#include "StmtMatmul.hpp"
#include "StmtMatmulAuto.hpp"
#include "StmtReadData.hpp"
#include "StmtReduce.hpp"
#include "StmtRepeat.hpp"
#include "StmtRNGrand.hpp"
#include "StmtRNGseed.hpp"
#include "StmtSendData.hpp"
#include "StmtSingle.hpp"
#include "TrimFat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// remove useless variables between
// kernels

TrimFat::TrimFat(const set< uint32_t >& useRegs,
                 const set< uint32_t >& useRegsWriteBack)
    : _useRegs(useRegs),
      _useRegsWriteBack(useRegsWriteBack),
      _createVars() { }

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

        if (_createVars.count(stmtPtr))
        {
            const uint32_t varNum = _createVars[stmtPtr];

            const bool trimStmt = _useRegs.count(varNum) &&
                                  ! _useRegsWriteBack.count(varNum);

            if (trimStmt)
            {
                delete stmtPtr;
            }
            else
            {
                newStmts.push_back(stmtPtr);
            }
        }
        else
        {
            newStmts.push_back(stmtPtr);
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
    // only interested in trace variables
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();

        _createVars[ &s ] = varNum;
    }
}

void TrimFat::visit(StmtExtension& s) { }

void TrimFat::visit(StmtExtensionAuto& s) { }

void TrimFat::visit(StmtIdSpace& s) { }

void TrimFat::visit(StmtIndex& s) { }

void TrimFat::visit(StmtLiteral& s) { }

void TrimFat::visit(StmtMatmul& s) { }

void TrimFat::visit(StmtMatmulAuto& s) { }

void TrimFat::visit(StmtReadData& s) { }

void TrimFat::visit(StmtReduce& s) { }

void TrimFat::visit(StmtRepeat& s) { }

void TrimFat::visit(StmtRNGrand& s) { }

void TrimFat::visit(StmtRNGseed& s) { }

void TrimFat::visit(StmtSendData& s) { }

void TrimFat::visit(StmtSingle& s) { }

}; // namespace chai_internal
