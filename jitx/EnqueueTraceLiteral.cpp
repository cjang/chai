// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"
#include "EnqueueTrace.hpp"
#include "PrecType.hpp"
#include "StmtLiteral.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtLiteral& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // LHS variable must be a memory buffer. It must be writable. The only
    // way it could be an image is if it were also an argument to matmul().
    // Then it would also appear on the RHS. So it must also be readable.
    // As it must be readable and writable, it can not be an image. This
    // implies the vector length can never be zero.
    _lhsVectorLength = getVectorLength(s.lhsVariable());
    _lhsPrecision = s.lhsVariable()->precision();

    // stream dimensions come from the left hand side
    const size_t lhsWidth = s.lhsVariable()->W();
    const size_t lhsHeight = s.lhsVariable()->H();

    // if context is the global index space, then every statement with a
    // stream larger than one element needs a loop
    const bool everyStmtNeedsLoop
        = _subs.top()->isGlobalIndexTiles() &&
          lhsWidth * lhsHeight > _lhsVectorLength;

    // check if a loop is necessary over the stream in the statement
    const bool needLoop = everyStmtNeedsLoop;

    // new subscript for loop
    ForLoopSubscript newSub( lhsWidth / _lhsVectorLength,
                             lhsHeight );

    // is a loop 1D or 2D?
    const bool useLoopH = 1 != lhsHeight;

    // begin loop
    if (needLoop)
    {
        // for-loop index for height
        const size_t loopIdxH = useLoopH
                                    ? _func->loopBegin(
                                                 0,
                                                 lhsHeight )
                                    : -1;

        // for-loop index for width
        const size_t loopIdxW = _func->loopBegin(
                                           0,
                                           lhsWidth < _lhsVectorLength
                                              ? 1
                                              : lhsWidth / _lhsVectorLength );

        // for-loop subscript
        newSub.setLoopVars(loopIdxW, loopIdxH);
        newSub.setGlobalIndexTiles();

        _subs.push(&newSub);
    }

    // LHS variable dimensions in subscript for index arithmetic
    _subs.top()->setVariable( lhsWidth,
                              lhsHeight,
                              getVectorLength(s.lhsVariable()) );

    // get literal value to assign
    double rhsVal;
    if (s.litdataPtr())
    {
        switch (s.litdataPtr()->precision())
        {
            case (PrecType::UInt32) :
                rhsVal = s.litdataPtr()->uintValue();
                break;
            case (PrecType::Int32) :
                rhsVal = s.litdataPtr()->intValue();
                break;
            case (PrecType::Float) :
                rhsVal = s.litdataPtr()->floatValue();
                break;
            case (PrecType::Double) :
                rhsVal = s.litdataPtr()->doubleValue();
                break;
        }
    }
    if (s.scalarPtr())
    {
        switch (s.scalarPtr()->precision())
        {
            case (PrecType::UInt32) :
                rhsVal = s.scalarPtr()->uintValue();
                break;
            case (PrecType::Int32) :
                rhsVal = s.scalarPtr()->intValue();
                break;
            case (PrecType::Float) :
                rhsVal = s.scalarPtr()->floatValue();
                break;
            case (PrecType::Double) :
                rhsVal = s.scalarPtr()->doubleValue();
                break;
        }
    }

    // get LHS variable (kernel generation variable, not the AST variable)
    bool isPrivateVar = false;
    Variable* lhsVar = NULL;
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();
        if (_xid->traceUseRegister().count(varNum))
        {
            isPrivateVar = true;
            lhsVar = _func->traceVarPrivate(varNum);
        }
        else
        {
            lhsVar = _func->traceVar(varNum);
        }
    }
    else
    {
        const AstVariable* varPtr = s.lhsVariable();
        lhsVar = _func->splitVar(varPtr);
    }

    // add new assignment statement
    _func->assignNumber(lhsVar, isPrivateVar ? NULL : _subs.top(), rhsVal);

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

}; // namespace chai_internal
