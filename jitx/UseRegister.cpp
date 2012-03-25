// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
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
#include "UseRegister.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// accumulate in private variable and
// assign to memory at the end

void UseRegister::updateRW(const AstVariable* stmtLHS)
{
    // all trace variables in current statement
    set< uint32_t > traceVars;

    // LHS trace variable
    if (stmtLHS->isTraceVariable())
    {
        const uint32_t varNum = stmtLHS->variable();

        traceVars.insert(varNum);

        // LHS is always potential candidate for register use
        _traceInclude.insert(varNum);

        // write to LHS
        if (_traceWrite.count(varNum))
            _traceWrite[varNum]++;
        else
            _traceWrite[varNum] = 1;
    }

    // RHS trace variables
    set< uint32_t > traceRHS;
    for (set< const AstVariable* >::const_iterator
         it = _stmtRHS.begin();
         it != _stmtRHS.end();
         it++)
    {
        if ((*it)->isTraceVariable())
        {
            const uint32_t varNum = (*it)->variable();

            traceVars.insert(varNum);
            traceRHS.insert(varNum);

            // read from RHS
            if (_traceRead.count(varNum))
                _traceRead[varNum]++;
            else
                _traceRead[varNum] = 1;
        }
    }

    // LHS variable also appears on RHS
    if (stmtLHS->isTraceVariable())
    {
        const uint32_t varNum = stmtLHS->variable();

        if (traceRHS.count(varNum))
        {
            if (_traceReadWrite.count(varNum))
                _traceReadWrite[varNum]++;
            else
                _traceReadWrite[varNum] = 1;
        }
    }

    // iterate over statement trace variables
    for (set< uint32_t >::const_iterator
         it = traceVars.begin();
         it != traceVars.end();
         it++)
    {
        const uint32_t varNum = *it;

        // first statement
        if (0 == _firstIsAssign.count(varNum))
        {
            // variable appears on LHS only
            _firstIsAssign[varNum] = 0 == traceRHS.count(varNum);
        }
    }
}

void UseRegister::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
        v.getArg(i)->accept(*this);
}

UseRegister::UseRegister(void) { }

void UseRegister::edit(StmtIdSpace& xid)
{
    // iterate over and traverse within kernel statements
    for (vector< Stmt* >::const_iterator
         it = xid.stuffInside().begin();
         it != xid.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }

    // candidate variables for private registers
    set< uint32_t > traceCandidates;

    for (map< uint32_t, size_t >::const_iterator
         it = _traceRead.begin();
         it != _traceRead.end();
         it++)
    {
        if ((*it).second > 1) // no benefit unless more than one read
        {
            const uint32_t varNum = (*it).first;

            // must be at least one write
            if (_traceWrite.count(varNum) && _traceWrite[varNum] >= 1)
            {
                // must be included
                if (_traceInclude.count(varNum))
                {
                    traceCandidates.insert(varNum);
                }
            }
        }
    }

    // exclude variables inside reductions or gathers
    for (set< uint32_t >::const_iterator
         it = _traceExclude.begin();
         it != _traceExclude.end();
         it++)
    {
        traceCandidates.erase(*it);
    }

    // only interested if first statement is an assignment
    for (map< uint32_t, bool >::const_iterator
         it = _firstIsAssign.begin();
         it != _firstIsAssign.end();
         it++)
    {
        if (! (*it).second)
            traceCandidates.erase((*it).first);
    }

    set< const Stmt* > removeBarriers;

    // check for any memory barriers that need to be removed
    for (set< const StmtBarrier* >::const_iterator
         it = _trackBarriers.begin();
         it != _trackBarriers.end();
         it++)
    {
        const StmtBarrier* sb = *it;

        bool commonVarFound = false;

        for (set< uint32_t >::const_iterator
             jt = sb->traceVars().begin();
             jt != sb->traceVars().end();
             jt++)
        {
            if (traceCandidates.count(*jt))
            {
                commonVarFound = true;
                break;
            }
        }

        if (commonVarFound)
            removeBarriers.insert(sb);
    }

    // remove memory barriers
    if (! removeBarriers.empty())
    {
        vector< Stmt* > newStmts;

        for (vector< Stmt* >::const_iterator
             it = xid.stuffInside().begin();
             it != xid.stuffInside().end();
             it++)
        {
            Stmt* stmtPtr = *it;

            if (0 == removeBarriers.count(stmtPtr))
                newStmts.push_back(stmtPtr);
            else
                delete stmtPtr;
        }

        xid.replaceStuff(newStmts);
    }

    // not all registers are written back to global memory
    set< uint32_t > writeBack;
    for (set< uint32_t >::const_iterator
         it = traceCandidates.begin();
         it != traceCandidates.end();
         it++)
    {
        const uint32_t varNum = *it;

        if (1 == _traceWrite[varNum] &&   // first assignment is only one
            0 == _traceReadWrite[varNum]) // variable value never changes
        {
            // do not write back
        }
        else
        {
            writeBack.insert(varNum);
        }
    }

    // these variables will have registers
    xid.traceUseRegister(traceCandidates, writeBack);
}

////////////////////////////////////////
// VisitAst

void UseRegister::visit(AstAccum& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstArrayMem& v)
{
}

void UseRegister::visit(AstCond& v)
{
    descendAst(v);
}

void UseRegister::visit(AstConvert& v)
{
    descendAst(v);
}

void UseRegister::visit(AstDotprod& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstExtension& v)
{
    descendAst(v);
}

void UseRegister::visit(AstFun1& v)
{
    descendAst(v);
}

void UseRegister::visit(AstFun2& v)
{
    descendAst(v);
}

void UseRegister::visit(AstFun3& v)
{
    descendAst(v);
}

void UseRegister::visit(AstGather& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstIdxdata& v)
{
}

void UseRegister::visit(AstLitdata& v)
{
}

void UseRegister::visit(AstMakedata& v)
{
}

void UseRegister::visit(AstMatmulMM& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstMatmulMV& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstMatmulVM& v)
{
    _trackContained.push(&v);
    descendAst(v);
    _trackContained.pop();
}

void UseRegister::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void UseRegister::visit(AstReadout& v)
{
}

void UseRegister::visit(AstRNGnormal& v)
{
}

void UseRegister::visit(AstRNGuniform& v)
{
}

void UseRegister::visit(AstScalar& v)
{
}

void UseRegister::visit(AstTranspose& v)
{
    descendAst(v);
}

void UseRegister::visit(AstVariable& v)
{
    // appears on RHS
    _stmtRHS.insert(&v);

    // only interested in trace variables
    if (v.isTraceVariable())
    {
        const uint32_t varNum = v.variable();

        // variables contained inside a reduction or gather
        if (_trackContained.empty())
        {
            // candidate for using register
            _traceInclude.insert(varNum);
        }
        else
        {
            // ignore pure stream variable inside reduction or gather
            _traceExclude.insert(varNum);
        }
    }
}

////////////////////////////////////////
// VisitStmt

void UseRegister::visit(StmtBarrier& s)
{
    // track for later removal if necessary
    _trackBarriers.insert(&s);
}

void UseRegister::visit(StmtCompound& s)
{
    // need to visit contained statements
    for (vector< Stmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void UseRegister::visit(StmtCreateData& s)
{
    // leave this alone, not changing memory management
}

void UseRegister::visit(StmtExtension& s)
{
    // leave this alone, is separate kernel
}

void UseRegister::visit(StmtExtensionAuto& s)
{
    // leave this alone, meta-kernel
}

void UseRegister::visit(StmtIdSpace& s)
{
    // should never happen
}

void UseRegister::visit(StmtIndex& s)
{
    _stmtRHS.clear();

    updateRW(s.lhsVariable());
}

void UseRegister::visit(StmtLiteral& s)
{
    _stmtRHS.clear();

    updateRW(s.lhsVariable());
}

void UseRegister::visit(StmtMatmul& s)
{
    // leave this alone, is separate kernel
}

void UseRegister::visit(StmtMatmulAuto& s)
{
    // leave this alone, meta-kernel
}

void UseRegister::visit(StmtReadData& s)
{
    // leave this alone, meta-kernel
}

void UseRegister::visit(StmtReduce& s)
{
    _stmtRHS.clear();

    if (s.accumPtr()) s.accumPtr()->accept(*this);
    if (s.dotprodPtr()) s.dotprodPtr()->accept(*this);

    updateRW(s.lhsVariable());
}

void UseRegister::visit(StmtRepeat& s)
{
    // need to visit contained statements
    s.stuffInside()->accept(*this);
}

void UseRegister::visit(StmtRNGrand& s)
{
    // leave this alone, no GPU support for RNG
}

void UseRegister::visit(StmtRNGseed& s)
{
    // leave this alone, no GPU support for RNG
}

void UseRegister::visit(StmtSendData& s)
{
    // leave this alone, meta-kernel
}

void UseRegister::visit(StmtSingle& s)
{
    _stmtRHS.clear();

    s.lhsVariable()->getArg(0)->accept(*this);

    updateRW(s.lhsVariable());
}

}; // namespace chai_internal
