// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>
#include <sstream>
#include <vector>

#include "AstAccum.hpp"
#include "AstDotprod.hpp"
#include "AstVariable.hpp"
#include "Enqueue.hpp"
#include "StmtReduce.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtReduce& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // LHS variable must be a memory buffer. It must be writable. The only
    // way it could be an image is if it were also an argument to matmul().
    // Then it would also appear on the RHS. So it must also be readable.
    // As it must be readable and writable, it can not be an image. This
    // implies the vector length can never be zero.
    _lhsVecLength = getVecLength(s.lhsVariable());
    _lhsPrec = s.lhsVariable()->prec();

    // vector length for private reduction variable
    size_t redVecLen = _minVecLength;
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin(); it != s.rhsVariable().end(); it++)
    {
        const size_t vecLen = getVecLength(*it);
        if (vecLen > redVecLen)
            redVecLen = vecLen;
    }

    // get reduction variable, this is managed by the function object
    AddrMem* reduceVar = _func->getPrivateVar( s.lhsVariable()->prec(),
                                               redVecLen );

    // set accumulation variable to zero
    _func->assignZero(reduceVar);

    // RHS dimensions for the reduction loop
    const size_t rhsWidth = s.rhsW();
    const size_t rhsHeight = s.rhsH();

    // begin outer loop down rows
    const bool useLoopH = 1 != rhsHeight;
    const size_t loopIndexH = useLoopH
                                  ? _func->loopBegin(
                                               0,
                                               rhsHeight )
                                  : -1;

    // begin inner loop across rows
    const size_t loopIndexW = _func->loopBegin(
                                         0,
                                         rhsWidth < redVecLen
                                             ? 1
                                             : rhsWidth / redVecLen );

    // new for-loop subscript
    ForLoopSubscript newSub( rhsWidth / redVecLen,
                             rhsHeight );

    newSub.setLoopVars(loopIndexW, loopIndexH);
    if (_subs.top()->isOuterLoopTiles())
    {
        newSub.setOuterLoopTiles( _subs.top()->getOuterLoopTileVar() );
    }
    if (_subs.top()->isGlobalIndexTiles())
    {
        newSub.setGlobalIndexTiles();
    }
    _subs.push(&newSub);

    // clear RHS statement string
    _rhsStmt.str(string());

    // disable the heuristic hack with dot() for scalars
    _scalarToScalar = true;

    // descend into body of loop here
    if (s.accumPtr()) s.accumPtr()->accept(*this);
    if (s.dotprodPtr()) s.dotprodPtr()->accept(*this);

    _scalarToScalar = false;

    // accumulation statement inside for-loop
    _func->incrementStatement(reduceVar, _rhsStmt.str());

    // end loops
    if (useLoopH) _func->loopEnd();
    _func->loopEnd();
    _subs.pop();

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

    // is the LHS variable in a read_scalar() statement?
    const bool isReadoutVar
        = ( s.lhsVariable()->isTraceVariable() &&
            0 == _xid->getReadoutDim(s.lhsVariable()->variable()) );

    // assigning accumulated scalar value so no array subscripts involved
    FixedSubscript newSubLHS(0);
    if (isReadoutVar) newSubLHS.setReadScalar();
    if (_subs.top()->isOuterLoopTiles())
    {
        newSubLHS.setOuterLoopTiles( _subs.top()->getOuterLoopTileVar() );
    }
    if (_subs.top()->isGlobalIndexTiles())
    {
        newSubLHS.setGlobalIndexTiles();
    }

    // special case if LHS variable is for read_scalar
    const size_t lhsVecLen = getVecLength(s.lhsVariable());

    // accumulation is mean, need to take average
    const size_t divideBy = rhsWidth * rhsHeight;
    const bool isDivideBy = s.accumPtr() &&
                            s.accumPtr()->takeAvg() &&
                            divideBy > 1;

    // reduction variable identifier name
    _rhsStmt.str(string());
    reduceVar->identifierName(_rhsStmt);
    const string redVarName = _rhsStmt.str();

    if (_func->isPrivate(lhsVar))
    {
        // assign to a private variable (not a stream so no subscript)
        stringstream ss;
        ss << redVarName;
        if (isDivideBy) ss << "/ " << divideBy;
        _func->assignStatement(lhsVar, ss.str());
    }
    else // assign to a stream so need subscript
    {
        newSubLHS.setVariable( s.lhsVariable()->W(),
                               s.lhsVariable()->H(),
                               lhsVecLen );


        // the LHS and RHS variables may be different vector lengths
        if (lhsVecLen < redVecLen)
        {
            vector< string > reduceComponents;
            switch (lhsVecLen)
            {
                case (1) :
                    for (size_t i = 0; i < redVecLen; i++)
                    {
                        stringstream ss;
                        ss << ".s" << i;
                        reduceComponents.push_back(ss.str());
                    }
                    break;

                case (2) :
                    reduceComponents.push_back(".lo");
                    reduceComponents.push_back(".hi");
                    break;

                // case (4) is impossible as lhsVecLen < redVecLen
            }

            // must slice and accumulate the reduction variable to the LHS
            for (vector< string >::const_iterator
                 it = reduceComponents.begin();
                 it != reduceComponents.end();
                 it++)
            {
                stringstream ss;
                ss << redVarName << *it;
                if (isDivideBy) ss << " / " << divideBy;
                _func->incrementStatement(lhsVar, &newSubLHS, ss.str());
            }
        }
        else if (lhsVecLen > redVecLen)
        {
            // must slice the LHS to fit the smaller reduction variable
            newSubLHS.setMixedVecLength(redVecLen);
            newSubLHS.setMixedComponentIdx(0);
            _func->assignStatement(lhsVar, &newSubLHS, redVarName);
        }
        else // lhsVecLen == redVecLen
        {
            // direct assignment is ok
            stringstream ss;
            ss << redVarName;
            if (isDivideBy) ss << " / " << divideBy;
            _func->assignStatement(lhsVar, &newSubLHS, ss.str());
        }
    }
}

}; // namespace chai_internal
