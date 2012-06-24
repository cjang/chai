// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <set>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "EnqueueTrace.hpp"
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

void EnqueueTrace::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

void EnqueueTrace::boundVectorLength(void)
{
    size_t minlen = 4; // largest possible
    size_t maxlen = 1; // smallest possible

    for (set< uint32_t >::const_iterator
         it = _xid->traceArgs().begin();
         it != _xid->traceArgs().end();
         it++)
    {
        const uint32_t varNum = *it;

        size_t vlen = getVectorLength(varNum);

        if (0 == vlen)
        {
            const size_t prec = _xid->getPrecision(varNum);
            vlen = PrecType::vecLength(prec);
        }

        if (vlen < minlen) minlen = vlen;
        if (vlen > maxlen) maxlen = vlen;
    }

    for (set< const AstVariable* >::const_iterator
         it = _xid->splitArgs().begin();
         it != _xid->splitArgs().end();
         it++)
    {
        const AstVariable* varPtr = *it;

        size_t vlen = getVectorLength(varPtr);

        if (0 == vlen)
        {
            const size_t prec = _xid->getPrecision(varPtr);
            vlen = PrecType::vecLength(prec);
        }

        if (vlen < minlen) minlen = vlen;
        if (vlen > maxlen) maxlen = vlen;
    }

    _minVectorLength = minlen;
    _maxVectorLength = maxlen;
}

size_t EnqueueTrace::getVectorLength(const uint32_t varNum)
{
    const size_t vlen = _jitMemo.getVectorLength(
                                     varNum,
                                     _xid->getPrecision(varNum) );

    // apply transposed array subscript constraint to memory buffers only
    return ( 0 != vlen && _xid->scalarVectorLength() )
               ? 1
               : vlen;
}

size_t EnqueueTrace::getVectorLength(const AstVariable* varPtr)
{
    const size_t vlen = _jitMemo.getVectorLength( varPtr );

    // apply transposed array subscript constraint to memory buffers only
    return ( 0 != vlen && _xid->scalarVectorLength() )
               ? 1
               : vlen;
}

EnqueueTrace::EnqueueTrace(VectorTrace& vt,
                           MemManager& mm,
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

EnqueueTrace::~EnqueueTrace(void)
{
    delete _printer;
}

bool EnqueueTrace::isOk(void) const
{
    return ! _failure;
}

}; // namespace chai_internal
