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
#include "ElideInternal.hpp"
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
#include "StmtReadData.hpp"
#include "StmtReduce.hpp"
#include "StmtRepeat.hpp"
#include "StmtSendData.hpp"
#include "StmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// variables lexically scoped within
// kernel are internal and elided

void ElideInternal::replaceAst(BaseAst& v,
                               const size_t argIndex)
{
    BaseAst* argPtr = v.getArg(argIndex);

    if (_trackVar.count(argPtr))
    {
        const uint32_t varNum = _trackVar[argPtr];

        if (_editVars.count(varNum))
        {
            v.replaceArg(argIndex, _editVars[varNum]);
        }
    }
}

void ElideInternal::replaceAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
        replaceAst(v, i);
}

void ElideInternal::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
        v.getArg(i)->accept(*this);
}

void ElideInternal::keepVariable(const AstVariable* varPtr)
{
    if (varPtr->isTraceVariable())
    {
        const uint32_t varNum = varPtr->variable();

        // treat reduction differently, LHS variables are always memory objects
        _markKeep.insert( varNum );
    }
}

ElideInternal::ElideInternal(const set< uint32_t >& candidates)
    : _candidates(candidates) { }

void ElideInternal::edit(StmtIdSpace& xid)
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

        if (_markRemove.count(stmtPtr))
        {
            bool keepStmt = false;

            for (set< uint32_t >::const_iterator
                 jt = _markRemove[stmtPtr].begin();
                 jt != _markRemove[stmtPtr].end();
                 jt++)
            {
                if (_markKeep.count(*jt))
                {
                    keepStmt = true;
                }
            }

            if (keepStmt)
            {
                newStmts.push_back(stmtPtr);
            }
            else
            {
                for (set< uint32_t >::const_iterator
                     jt = _markRemove[stmtPtr].begin();
                     jt != _markRemove[stmtPtr].end();
                     jt++)
                {
                    xid.removeTraceArg(*jt);
                }

                delete stmtPtr;
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
// VisitAst

void ElideInternal::visit(AstAccum& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstArrayMem& v)
{
}

void ElideInternal::visit(AstCond& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstConvert& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstDotprod& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstExtension& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstFun1& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstFun2& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstFun3& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstGather& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstIdxdata& v)
{
}

void ElideInternal::visit(AstLitdata& v)
{
}

void ElideInternal::visit(AstMakedata& v)
{
}

void ElideInternal::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void ElideInternal::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void ElideInternal::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void ElideInternal::visit(AstMatmulVV& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstReadout& v)
{
}

void ElideInternal::visit(AstRNGnormal& v)
{
}

void ElideInternal::visit(AstRNGuniform& v)
{
}

void ElideInternal::visit(AstScalar& v)
{
}

void ElideInternal::visit(AstTranspose& v)
{
    descendAst(v);
    replaceAst(v);
}

void ElideInternal::visit(AstVariable& v)
{
    if (v.isTraceVariable())
    {
        const uint32_t varNum = v.variable();

        if (_candidates.count(varNum))
        {
            _trackVar[ &v ] = varNum;
        }
    }
}

////////////////////////////////////////
// VisitStmt

void ElideInternal::visit(StmtBarrier& s)
{
    // check the variables for this barrier
    if (s.containedIn(_candidates))
    {
        // barrier is unnecessary, this is for elided variables
        _markRemove[ &s ] = s.traceVars();
    }
}

void ElideInternal::visit(StmtCompound& s)
{
    // need to edit elided variables in contained statements
    for (vector< Stmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void ElideInternal::visit(StmtCreateData& s)
{
    // only interested in trace variables
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();

        if (_candidates.count(varNum))
        {
            // this variable is elided, no memory object should be created
            _markRemove[ &s ].insert( varNum );
        }
    }
}

void ElideInternal::visit(StmtExtension& s)
{
    // leave this alone, is separate kernel
}

void ElideInternal::visit(StmtExtensionAuto& s)
{
    // leave this alone, meta-kernel
}

void ElideInternal::visit(StmtGatherAuto& s)
{
    // leave this alone, meta-kernel
}

void ElideInternal::visit(StmtIdSpace& s)
{
    // should never happen
}

void ElideInternal::visit(StmtIndex& s)
{
    // always a trace variable
    const uint32_t varNum = s.lhsVariable()->variable();

    if (_candidates.count(varNum))
    {
        // this statement is for an elided variable
        _editVars[ varNum ] = s.lhsVariable()->getArg(0);
        _markRemove[ &s ].insert( varNum );
    }
}

void ElideInternal::visit(StmtLiteral& s)
{
    // always a trace variable
    const uint32_t varNum = s.lhsVariable()->variable();

    if (_candidates.count(varNum))
    {
        // this statement is for an elided variable
        _editVars[ varNum ] = s.lhsVariable()->getArg(0);
        _markRemove[ &s ].insert( varNum );
    }
}

void ElideInternal::visit(StmtMatmul& s)
{
    // leave this alone, is separate kernel
}

void ElideInternal::visit(StmtMatmulAuto& s)
{
    // leave this alone, meta-kernel
}

void ElideInternal::visit(StmtReadData& s)
{
    // leave this alone, meta-kernel
}

void ElideInternal::visit(StmtReduce& s)
{
    // need to edit elided variables
    if (s.accumPtr()) s.accumPtr()->accept(*this);
    if (s.dotprodPtr()) s.dotprodPtr()->accept(*this);

    // treat reduction differently, LHS variables are always memory objects
    keepVariable(s.lhsVariable());
}

void ElideInternal::visit(StmtRepeat& s)
{
    // need to edit elided variables in contained statements
    s.stuffInside()->accept(*this);

/*FIXME - remove this
    // any RNG variables inside reduction must be memory objects
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin();
         it != s.rhsVariable().end();
         it++)
    {
        AstVariable* v = *it;
        if (v->getValueFromRNG() && v->getForceWriteback())
        {
            keepVariable(v);
        }
    }
*/
}

void ElideInternal::visit(StmtSendData& s)
{
    // leave this alone, meta-kernel
}

void ElideInternal::visit(StmtSingle& s)
{
    // need to edit elided variables
    s.lhsVariable()->getArg(0)->accept(*this);

    // only interested in trace variables
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();

        if (_candidates.count(varNum))
        {
            // this statement is for an elided variable
            _editVars[ varNum ] = s.lhsVariable()->getArg(0);
            _markRemove[ &s ].insert( varNum );
        }
    }
}

}; // namespace chai_internal
