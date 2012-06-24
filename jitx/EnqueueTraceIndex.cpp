// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"
#include "EnqueueTrace.hpp"
#include "StmtIndex.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtIndex& s)
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
        = lhsWidth * lhsHeight > _lhsVectorLength;

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
                              _lhsVectorLength );

    // descend into the AST index object
    s.idxdataPtr()->accept(*this);

    // get LHS variable (kernel generation variable, not the AST variable)
    Variable* lhsVar = NULL;
    if (s.lhsVariable()->isTraceVariable())
    {
        const uint32_t varNum = s.lhsVariable()->variable();
        if (_xid->traceUseRegister().count(varNum))
        {
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

    // assign LHS with index value
    _func->assignStatement(lhsVar, _subs.top(), _rhsStmt.str());

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

}; // namespace chai_internal
