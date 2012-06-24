// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "AstVariable.hpp"
#include "EnqueueTrace.hpp"
#include "PrecType.hpp"
#include "StmtSingle.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtSingle& s)
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

    // RHS variable vector lengths
    _smallVectorLength = _largeVectorLength = _lhsVectorLength;
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin();
         it != s.rhsVariable().end();
         it++)
    {
        size_t vecLen = getVectorLength(*it);
        if (0 == vecLen)
        {
            const size_t prec = _xid->getPrecision(*it);
            vecLen = PrecType::vecLength(prec);
        }
        if (vecLen < _smallVectorLength) _smallVectorLength = vecLen;
        if (vecLen > _largeVectorLength) _largeVectorLength = vecLen;
    }

    // stream dimensions come from the left hand side
    const size_t lhsWidth = s.lhsVariable()->W();
    const size_t lhsHeight = s.lhsVariable()->H();

    // if context is the global index space, then every statement with a
    // stream larger than one element needs a loop
    const bool everyStmtNeedsLoop
        = _subs.top()->isGlobalIndexTiles() &&
          lhsWidth * lhsHeight > _smallVectorLength;

    const size_t padW
        = (0 != (lhsWidth % _smallVectorLength))
          ? _smallVectorLength * (1 + (lhsWidth / _smallVectorLength))
          : lhsWidth;

    // if context does not match the statement stream dimensions
    const bool streamSizeMismatch
        = _subs.top()->boundingWidth() != padW / _smallVectorLength ||
          _subs.top()->boundingHeight() != lhsHeight;

    // check if a loop is necessary over the stream in the statement
    const bool needLoop = everyStmtNeedsLoop || streamSizeMismatch;

    // is a loop 1D or 2D?
    const bool useLoopH = 1 != lhsHeight;

    // new subscript for loop
    ForLoopSubscript newSub( padW / _smallVectorLength,
                             lhsHeight);

    // begin loop
    if (needLoop)
    {
        // for-loop index for height
        const size_t loopIdxH = useLoopH
                                    ? _func->loopBegin( 0, lhsHeight )
                                    : -1;

        // for-loop index for width
        const size_t loopIdxW
            = _func->loopBegin( 0, lhsWidth < _smallVectorLength
                                       ? 1
                                       : padW / _smallVectorLength );

        // for-loop subscript
        newSub.setLoopVars(loopIdxW, loopIdxH);
        newSub.setGlobalIndexTiles();

        _subs.push(&newSub);
    }

    // mixed variable vector lengths require multiple statements and components
    set< size_t > componentIdxSet;
    if (_smallVectorLength == _largeVectorLength)
    {
        // all vector lengths are the same, nothing special required
        componentIdxSet.insert(-1);
    }
    else
    {
        // mixed variable vector lengths in statement
        _subs.top()->setMixedVectorLength(_smallVectorLength);

        switch (_smallVectorLength)
        {
            case (1) :
                if (2 == _largeVectorLength)
                {
                    componentIdxSet.insert(0); // .s0 for vector length 2
                    componentIdxSet.insert(1); // .s1 for vector length 2
                }
                else
                {
                    componentIdxSet.insert(0); // .s0 for vector length 2, 4
                    componentIdxSet.insert(1); // .s1 for vector length 2, 4
                    componentIdxSet.insert(2); // .s2 for vector length 4
                    componentIdxSet.insert(3); // .s3 for vector length 4
                }
                break;

            case (2) :
                componentIdxSet.insert(0); // .lo for vector length 4
                componentIdxSet.insert(1); // .hi for vector length 4
                break;

            // can not be 4 as must be strictly less than large vector length
        }
    }

    // loop over components
    for (set< size_t >::const_iterator
         it = componentIdxSet.begin();
         it != componentIdxSet.end();
         it++)
    {
        // set variable component
        if (-1 != *it) _subs.top()->setMixedComponentIdx(*it);

        // clear RHS statement string
        _rhsStmt.str(string());

        _scalarToScalar = s.scalarToScalar();

        // descend into RHS
        s.lhsVariable()->getArg(0)->accept(*this); // 0th arg is the RHS

        _scalarToScalar = false;

        // get LHS variable
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

        if (_func->isPrivate(lhsVar))
        {
            // add new assignment statement
            _func->assignStatement(lhsVar, NULL, _rhsStmt.str());
        }
        else
        {
            // LHS variable dimensions in subscript for index arithmetic
            _subs.top()->setVariable( lhsWidth,
                                      lhsHeight,
                                      _lhsVectorLength );

            // add new assignment statement
            _func->assignStatement(lhsVar, _subs.top(), _rhsStmt.str());
        }
    }

    _subs.top()->unsetMixedVectorLength();

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

}; // namespace chai_internal
