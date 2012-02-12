// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtIdSpace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

XStmtIdSpace::SpecialKernel XStmtIdSpace::convertSpecialK(
                                const XStmtTogether::SpecialKernel val ) const
{
    switch (val)
    {
        case (XStmtTogether::NOTHING_SPECIAL) :
            return NOTHING_SPECIAL;

        case (XStmtTogether::SPECIAL_MATMUL) :
            return SPECIAL_MATMUL;

        case (XStmtTogether::SPECIAL_MATMUL_AUTO) :
            return SPECIAL_MATMUL_AUTO;
    }
}

XStmtIdSpace::XStmtIdSpace(const XStmtTogether& together,
                           const size_t kernelNumber,
                           VectorTrace& vt)
    : _together(together),
      _stmts(together.stuffInside()),
      _traceArgs(together.traceArguments()),
      _splitArgs(together.splitArguments()),
      _traceLHS(together.traceLHS()),
      _traceRHS(together.traceRHS()),
      _splitLHS(together.splitLHS()),
      _splitRHS(together.splitRHS()),
      _traceTransposed(together.traceTransposed()),
      _splitTransposed(together.splitTransposed()),
      _traceGathered(together.traceGathered()),
      _splitGathered(together.splitGathered()),
      _prec32(together.isSinglePrecision()),
      _prec64(together.isDoublePrecision()),
      _streamW(together.streamW()),
      _streamH(together.streamH()),
      _hashCode(vt.hashCode()),
      _numTraces(vt.numTraces()),
      _kernelNumber(kernelNumber),
      _vt(vt),
      _specialK(convertSpecialK(together.specialK()))
{
    buoyancyNeutral();
}

XStmtIdSpace::~XStmtIdSpace(void)
{
    // there can be duplicate pointers in the vector
    set< XStmt* > uniqueStmts;

    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        uniqueStmts.insert(*it);
    }

    for (set< XStmt* >::const_iterator
         it = uniqueStmts.begin();
         it != uniqueStmts.end();
         it++)
    {
        delete *it;
    }
}

void XStmtIdSpace::setReadoutDim(
                       const std::map< uint32_t, size_t >& traceReadoutDim )
{
    _traceReadoutDim = traceReadoutDim;
}

size_t XStmtIdSpace::getReadoutDim(const uint32_t varNum)
{
    return _traceReadoutDim.count(varNum)
               ? _traceReadoutDim[varNum] // values are: 0, 1, 2
               : -1;
}

const set< uint32_t >& XStmtIdSpace::traceArgs(void) const
{
    return _traceArgs;
}

const set< const AstVariable* >& XStmtIdSpace::splitArgs(void) const
{
    return _splitArgs;
}

bool XStmtIdSpace::anyTransposed(void) const
{
    return ! _traceTransposed.empty() || ! _splitTransposed.empty();
}

bool XStmtIdSpace::traceTransposed(const uint32_t varNum) const
{
    return _traceTransposed.count(varNum);
}

bool XStmtIdSpace::splitTransposed(const AstVariable* varPtr) const
{
    return _splitTransposed.count(varPtr);
}

bool XStmtIdSpace::anyGathered(void) const
{
    return ! _traceGathered.empty() || ! _splitGathered.empty();
}

bool XStmtIdSpace::traceGathered(const uint32_t varNum) const
{
    return _traceGathered.count(varNum);
}

bool XStmtIdSpace::splitGathered(const AstVariable* varPtr) const
{
    return _splitGathered.count(varPtr);
}

bool XStmtIdSpace::isReadOnly(const uint32_t variable) const
{
    return _together.isReadOnly(variable);
}

bool XStmtIdSpace::isReadOnly(const AstVariable* variable) const
{
    return _together.isReadOnly(variable);
}

bool XStmtIdSpace::isWriteOnly(const uint32_t variable) const
{
    return _together.isWriteOnly(variable);
}

bool XStmtIdSpace::isWriteOnly(const AstVariable* variable) const
{
    return _together.isWriteOnly(variable);
}

bool XStmtIdSpace::isReadWrite(const uint32_t variable) const
{
    return _together.isReadWrite(variable);
}

bool XStmtIdSpace::isReadWrite(const AstVariable* variable) const
{
    return _together.isReadWrite(variable);
}

size_t XStmtIdSpace::getWidth(const uint32_t variable) const
{
    return _together.width(variable);
}

size_t XStmtIdSpace::getWidth(const AstVariable* variable) const
{
    return _together.width(variable);
}

size_t XStmtIdSpace::getHeight(const uint32_t variable) const
{
    return _together.height(variable);
}

size_t XStmtIdSpace::getHeight(const AstVariable* variable) const
{
    return _together.height(variable);
}

size_t XStmtIdSpace::getPrecision(const uint32_t variable) const
{
    return _together.precision(variable);
}

size_t XStmtIdSpace::getPrecision(const AstVariable* variable) const
{
    return _together.precision(variable);
}

bool XStmtIdSpace::isMixedPrecision(void) const
{
    return _prec32 && _prec64;
}

bool XStmtIdSpace::isSinglePrecision(void) const
{
    return _prec32;
}

bool XStmtIdSpace::isDoublePrecision(void) const
{
    return _prec64;
}

size_t XStmtIdSpace::streamW(void) const
{
    return _streamW;
}

size_t XStmtIdSpace::streamH(void) const
{
    return _streamH;
}

uint64_t XStmtIdSpace::hashCode(void) const
{
    return _hashCode;
}

size_t XStmtIdSpace::numTraces(void) const
{
    return _numTraces;
}

size_t XStmtIdSpace::kernelNumber(void) const
{
    return _kernelNumber;
}

VectorTrace& XStmtIdSpace::getVectorTrace(void)
{
    return _vt;
}

XStmtIdSpace::SpecialKernel XStmtIdSpace::specialK(void) const
{
    return _specialK;
}

bool XStmtIdSpace::swappable(const XStmt& other) const
{
    return false;
}

void XStmtIdSpace::accept(VisitXStmt& v)
{
    v.visit(*this);
}

const vector< XStmt* >& XStmtIdSpace::stuffInside(void) const
{
    return _stmts;
}

}; // namespace chai_internal
