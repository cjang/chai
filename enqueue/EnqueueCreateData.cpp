// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "AstVariable.hpp"
#include "Enqueue.hpp"
#include "PrecType.hpp"
#include "StmtCreateData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtCreateData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    const AstVariable* varPtr = s.lhsVariable();
    const uint32_t varNum = s.lhsVariable()->variable();

    const size_t   prec   = s.lhsVariable()->prec();
    const uint32_t val32u = s.uintValue();
    const int32_t  val32i = s.intValue();
    const float    val32  = s.floatValue();
    const double   val64  = s.doubleValue();

    if (s.outerLexicalScope())
    {
        // variable is in outermost lexical scope
        // therefore, it may span kernels or be referenced by clients

        if (s.initializeData())
        {
            // note all variable declarations reach here
            switch (prec)
            {
                case (PrecType::UInt32) :
                    if (! _jitMemo.createArrayTemp(varPtr, *_xid, val32u))
                        _failure = true;
                    break;
                case (PrecType::Int32) :
                    if (! _jitMemo.createArrayTemp(varPtr, *_xid, val32i))
                        _failure = true;
                    break;
                case (PrecType::Float) :
                    if (! _jitMemo.createArrayTemp(varPtr, *_xid, val32))
                        _failure = true;
                    break;
                case (PrecType::Double) :
                    if (! _jitMemo.createArrayTemp(varPtr, *_xid, val64))
                        _failure = true;
                    break;
            }
        }
        else
        {
            // note that only operation splits reach here
            if (! _jitMemo.createArrayTemp(varPtr, *_xid))
                _failure = true;
        }
    }
    else
    {
        // variable is inside a for-loop, therefore it is private

        size_t maxVecLen = _minVecLength;
        for (set< AstVariable* >::const_iterator
             it = s.underlyingVars().begin();
             it != s.underlyingVars().end();
             it++)
        {
            const size_t vecLen = getVecLength(*it);
            if (vecLen > maxVecLen)
                maxVecLen = vecLen;
        }

        AddrMem* newVar
            = varPtr->isTraceVariable()
                  ? _func->createPrivate(varNum, prec, maxVecLen)
                  : _func->createPrivate(varPtr, prec, maxVecLen);

        if (s.initializeData())
        {
            double val;

            switch (prec)
            {
                case (PrecType::UInt32) : val = val32u; break;
                case (PrecType::Int32) : val = val32i; break;
                case (PrecType::Float) : val = val32; break;
                case (PrecType::Double) : val = val64; break;
            }

            _func->assignNumber(newVar, val);
        }
    }
}

}; // namespace chai_internal
