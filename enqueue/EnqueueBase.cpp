// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <set>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "Enqueue.hpp"
#include "JustInTimeMemo.hpp"
#include "PrecType.hpp"
#include "SourceKernel.hpp"
#include "StandardEM.hpp"
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
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace as OpenCL

void Enqueue::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

void Enqueue::boundVecLength(void)
{
    size_t minLen = 4; // largest possible
    size_t maxLen = 1; // smallest possible

    for (set< uint32_t >::const_iterator
         it = _xid->traceArgs().begin(); it != _xid->traceArgs().end(); it++)
    {
        const uint32_t varNum = *it;

        size_t vecLen = getVecLength(varNum);

        if (0 == vecLen)
        {
            vecLen = PrecType::vecLength( _xid->getPrec(varNum) );
        }

        if (vecLen < minLen) minLen = vecLen;
        if (vecLen > maxLen) maxLen = vecLen;
    }

    for (set< const AstVariable* >::const_iterator
         it = _xid->splitArgs().begin(); it != _xid->splitArgs().end(); it++)
    {
        const AstVariable* varPtr = *it;

        size_t vecLen = getVecLength(varPtr);

        if (0 == vecLen)
        {
            vecLen = PrecType::vecLength( _xid->getPrec(varPtr) );
        }

        if (vecLen < minLen) minLen = vecLen;
        if (vecLen > maxLen) maxLen = vecLen;
    }

    _minVecLength = minLen;
    _maxVecLength = maxLen;
}

size_t Enqueue::getVecLength(const uint32_t varNum)
{
    const size_t vecLen = _jitMemo.getVecLength( varNum,
                                                 _xid->getPrec(varNum));

    // apply transposed array subscript constraint to memory buffers only
    return ( 0 != vecLen && _xid->scalarVecLength() )
               ? 1
               : vecLen;
}

size_t Enqueue::getVecLength(const AstVariable* varPtr)
{
    const size_t vecLen = _jitMemo.getVecLength( varPtr );

    // apply transposed array subscript constraint to memory buffers only
    return ( 0 != vecLen && _xid->scalarVecLength() )
               ? 1
               : vecLen;
}

Enqueue::Enqueue(VectorTrace& vt,
                 MemTrans& mm,
                 StandardEM& stdEM,
                 JustInTimeMemo& jitMemo)
    : _vt(vt),
      _memMgr(mm),
      _stdEM(stdEM),
      _jitMemo(jitMemo),
      _printer(NULL),
      _xid(NULL),
      _failure(false),
      _func(NULL),
      _scalarToScalar(false)
{
#ifdef __LOGGING_ENABLED__
    _printer = new PrintTrace(cerr);
#endif
}

Enqueue::~Enqueue(void)
{
    delete _printer;
}

bool Enqueue::isOk(void) const
{
    return ! _failure;
}

}; // namespace chai_internal
