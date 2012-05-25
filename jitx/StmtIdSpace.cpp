// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtIdSpace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

StmtIdSpace::SpecialKernel StmtIdSpace::convertSpecialK(
                               const GroupTogether::SpecialKernel val ) const
{
    switch (val)
    {
        case (GroupTogether::NOTHING_SPECIAL) :
            return NOTHING_SPECIAL;

        case (GroupTogether::SPECIAL_MATMUL) :
            return SPECIAL_MATMUL;

        case (GroupTogether::SPECIAL_MATMUL_AUTO) :
            return SPECIAL_MATMUL_AUTO;
    }
}

StmtIdSpace::StmtIdSpace(const GroupTogether& together,
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
      _scalarVectorLength(together.scalarVectorLength()),
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

StmtIdSpace::~StmtIdSpace(void)
{
    // there can be duplicate pointers in the vector
    set< Stmt* > uniqueStmts;

    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        uniqueStmts.insert(*it);
    }

    for (set< Stmt* >::const_iterator
         it = uniqueStmts.begin();
         it != uniqueStmts.end();
         it++)
    {
        delete *it;
    }
}

void StmtIdSpace::setReadoutDim(
                       const std::map< uint32_t, size_t >& traceReadoutDim )
{
    _traceReadoutDim = traceReadoutDim;
}

size_t StmtIdSpace::getReadoutDim(const uint32_t varNum)
{
    return _traceReadoutDim.count(varNum)
               ? _traceReadoutDim[varNum] // values are: 0, 1, 2
               : -1;
}

const set< uint32_t >& StmtIdSpace::traceUseRegister(void) const
{
    return _traceUseRegister;
}

const set< uint32_t >& StmtIdSpace::traceUseRegisterWriteBack(void) const
{
    return _traceUseRegisterWriteBack;
}

void StmtIdSpace::traceUseRegister(const set< uint32_t >& useRegisters,
                                   const set< uint32_t >& writeBack)
{
    _traceUseRegister = useRegisters;
    _traceUseRegisterWriteBack = writeBack;
}

void StmtIdSpace::removeTraceArg(const uint32_t varNum)
{
    _traceArgs.erase(varNum);
}

const set< uint32_t >& StmtIdSpace::traceArgs(void) const
{
    return _traceArgs;
}

const set< const AstVariable* >& StmtIdSpace::splitArgs(void) const
{
    return _splitArgs;
}

bool StmtIdSpace::scalarVectorLength(void) const
{
    return _scalarVectorLength;
}

bool StmtIdSpace::isReadOnly(const uint32_t variable) const
{
    return _together.isReadOnly(variable);
}

bool StmtIdSpace::isReadOnly(const AstVariable* variable) const
{
    return _together.isReadOnly(variable);
}

bool StmtIdSpace::isWriteOnly(const uint32_t variable) const
{
    return _together.isWriteOnly(variable);
}

bool StmtIdSpace::isWriteOnly(const AstVariable* variable) const
{
    return _together.isWriteOnly(variable);
}

bool StmtIdSpace::isReadWrite(const uint32_t variable) const
{
    return _together.isReadWrite(variable);
}

bool StmtIdSpace::isReadWrite(const AstVariable* variable) const
{
    return _together.isReadWrite(variable);
}

size_t StmtIdSpace::getWidth(const uint32_t variable) const
{
    return _together.width(variable);
}

size_t StmtIdSpace::getWidth(const AstVariable* variable) const
{
    return _together.width(variable);
}

size_t StmtIdSpace::getHeight(const uint32_t variable) const
{
    return _together.height(variable);
}

size_t StmtIdSpace::getHeight(const AstVariable* variable) const
{
    return _together.height(variable);
}

size_t StmtIdSpace::getPrecision(const uint32_t variable) const
{
    return _together.precision(variable);
}

size_t StmtIdSpace::getPrecision(const AstVariable* variable) const
{
    return _together.precision(variable);
}

size_t StmtIdSpace::streamW(void) const
{
    return _streamW;
}

size_t StmtIdSpace::streamH(void) const
{
    return _streamH;
}

uint64_t StmtIdSpace::hashCode(void) const
{
    return _hashCode;
}

size_t StmtIdSpace::numTraces(void) const
{
    return _numTraces;
}

size_t StmtIdSpace::kernelNumber(void) const
{
    return _kernelNumber;
}

VectorTrace& StmtIdSpace::getVectorTrace(void)
{
    return _vt;
}

StmtIdSpace::SpecialKernel StmtIdSpace::specialK(void) const
{
    return _specialK;
}

bool StmtIdSpace::swappable(const Stmt& other) const
{
    return false;
}

void StmtIdSpace::accept(VisitStmt& v)
{
    v.visit(*this);
}

const vector< Stmt* >& StmtIdSpace::stuffInside(void) const
{
    return _stmts;
}

void StmtIdSpace::replaceStuff(const vector< Stmt* >& newStmts)
{
    _stmts = newStmts;
}

}; // namespace chai_internal
