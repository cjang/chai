// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "OCLhacks.hpp"
#include "XStmtBarrier.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtExtension.hpp"
#include "XStmtExtensionAuto.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtMatmulAuto.hpp"
#include "XStmtReadData.hpp"
#include "XStmtReduce.hpp"
#include "XStmtRepeat.hpp"
#include "XStmtRNGrand.hpp"
#include "XStmtRNGseed.hpp"
#include "XStmtSendData.hpp"
#include "XStmtSingle.hpp"
#include "XStmtTogether.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// variable description

XStmtTogether::VariableDesc::VariableDesc(void)
    : _variableObj(NULL),
      _isArgument(false),
      _appearsOnLHS(false),
      _appearsOnRHS(false) { }

XStmtTogether::VariableDesc::VariableDesc(const AstVariable* obj,
                                          const bool isArg)
    : _variableObj(obj),
      _isArgument(isArg),
      _appearsOnLHS(false),
      _appearsOnRHS(false) { }

XStmtTogether::VariableDesc::VariableDesc(const VariableDesc& other)
    : _variableObj(other._variableObj),
      _isArgument(other._isArgument),
      _appearsOnLHS(other._appearsOnLHS),
      _appearsOnRHS(other._appearsOnRHS) { }

XStmtTogether::VariableDesc& XStmtTogether::VariableDesc::operator= (
                                 const VariableDesc& other )
{
    _variableObj = other._variableObj;
    _isArgument = other._isArgument;
    _appearsOnLHS = other._appearsOnLHS;
    _appearsOnRHS = other._appearsOnRHS;

    return *this;
}

bool XStmtTogether::VariableDesc::isArgument(void) const
{
    return _isArgument;
}

bool XStmtTogether::VariableDesc::isSameDataAcrossTraces(void) const
{
    return _variableObj->isSameDataAcrossTraces();
}

bool XStmtTogether::VariableDesc::isReadOnly(const bool appearsOnLHS,
                                             const bool appearsOnRHS) const
{
    return _variableObj->isReadOnly(appearsOnLHS, appearsOnRHS);
}

bool XStmtTogether::VariableDesc::isWriteOnly(const bool appearsOnLHS,
                                              const bool appearsOnRHS) const
{
    return _variableObj->isWriteOnly(appearsOnLHS, appearsOnRHS);
}

bool XStmtTogether::VariableDesc::isReadWrite(const bool appearsOnLHS,
                                              const bool appearsOnRHS) const
{
    return _variableObj->isReadWrite(appearsOnLHS, appearsOnRHS);
}

bool XStmtTogether::VariableDesc::getAppearsOnLHS(void) const
{
    return _appearsOnLHS;
}

bool XStmtTogether::VariableDesc::getAppearsOnRHS(void) const
{
    return _appearsOnRHS;
}

void XStmtTogether::VariableDesc::setAppearsOnLHS(void)
{
    _appearsOnLHS = true;
}

void XStmtTogether::VariableDesc::setAppearsOnRHS(void)
{
    _appearsOnRHS = true;
}

bool XStmtTogether::VariableDesc::isReadOnly(void) const
{
    return isReadOnly(_appearsOnLHS, _appearsOnRHS);
}

bool XStmtTogether::VariableDesc::isWriteOnly(void) const
{
    return isWriteOnly(_appearsOnLHS, _appearsOnRHS);
}

bool XStmtTogether::VariableDesc::isReadWrite(void) const
{
    return isReadWrite(_appearsOnLHS, _appearsOnRHS);
}

size_t XStmtTogether::VariableDesc::width(void) const
{
    return _variableObj->W();
}

size_t XStmtTogether::VariableDesc::height(void) const
{
    return _variableObj->H();
}

size_t XStmtTogether::VariableDesc::precision(void) const
{
    return _variableObj->precision();
}

////////////////////////////////////////
// data movement and code statement
// grouping

bool XStmtTogether::isExtension(void)
{
    if (_prevIsExtension)
    {
        _prevIsExtension = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isMatmul(void)
{
    if (_prevIsMatmul)
    {
        _prevIsMatmul = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReduction(void)
{
    if (_prevIsReduction)
    {
        _prevIsReduction = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isSpecial(void)
{
    if (_reductionIsSpecial)
    {
        return isExtension() || isMatmul() || isReduction();
    }
    else
    {
        return isExtension() || isMatmul();
    }
}

bool XStmtTogether::isEmpty(void) const
{
    return _togetherList.empty();
}

bool XStmtTogether::isPragmas(void) const
{
    // check for non-empty list with no dimensions yet
    return 0 == _streamW && 0 == _streamH && !_togetherList.empty();
}

bool XStmtTogether::isCode(void) const
{
    // check for dimensions which indicates there is code
    return 0 != _streamW || 0 != _streamH;
}

void XStmtTogether::boundIndex(const size_t w, const size_t h)
{
    if (w > _streamW) _streamW = w;
    if (h > _streamH) _streamH = h;
}

void XStmtTogether::boundIndex(const XStmtTogether& t)
{
    boundIndex(t.streamW(), t.streamH());
}

void XStmtTogether::boundIndex(const XStmt& s)
{
    boundIndex(s.lhsVariable()->W(), s.lhsVariable()->H());
}

void XStmtTogether::setIndex(const XStmtReadData& s)
{
    _streamW = s.lhsVariable()->W();
    _streamH = s.lhsVariable()->H();
}

void XStmtTogether::pushList(XStmt& s)
{
    // insert barrier if RHS references earlier LHS
    set< uint32_t >     traceMatch;
    set< AstVariable* > splitMatch;
    if ( checkBarrier(s.rhsVariable(),
                      traceMatch,
                      splitMatch) )
    {
        _togetherList.push_back( new XStmtBarrier(traceMatch,
                                                  splitMatch) );
    }

    // add current statement
    _togetherList.push_back(&s);

    // RHS variables
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin();
         it != s.rhsVariable().end();
         it++)
    {
        insertVariable(*it, false, true);
    }

    // LHS variable
    //insertVariable(s.lhsVariable(), true, false);
    insertVariable(s.lhsVariable(), s.trackLHS(), false);

    barrierVariable(s.lhsVariable());
}

// used by compound and repeat statements
void XStmtTogether::pushList(XStmt& s, XStmtTogether& t)
{
    _togetherList.push_back(&s);

    for (map< uint32_t, VariableDesc >::iterator
         it = t._traceVarDesc.begin();
         it != t._traceVarDesc.end();
         it++)
    {
        const uint32_t varNum = (*it).first;
        VariableDesc& varDesc = (*it).second;

        if (varDesc.getAppearsOnLHS()) _traceVarDesc[varNum].setAppearsOnLHS();
        if (varDesc.getAppearsOnRHS()) _traceVarDesc[varNum].setAppearsOnRHS();
    }

    for (map< const AstVariable*, VariableDesc >::iterator
         it = t._splitVarDesc.begin();
         it != t._splitVarDesc.end();
         it++)
    {
        const AstVariable* varPtr = (*it).first;
        VariableDesc& varDesc = (*it).second;

        if (varDesc.getAppearsOnLHS()) _splitVarDesc[varPtr].setAppearsOnLHS();
        if (varDesc.getAppearsOnRHS()) _splitVarDesc[varPtr].setAppearsOnRHS();
    }

    // kernel arguments
    _traceArgs.insert( t._traceArgs.begin(), t._traceArgs.end() );
    _splitArgs.insert( t._splitArgs.begin(), t._splitArgs.end() );

    // barrier variables
    _traceBarrier.insert( t._traceBarrier.begin(), t._traceBarrier.end() );
    _splitBarrier.insert( t._splitBarrier.begin(), t._splitBarrier.end() );

    // track variable sideness
    _traceLHS.insert( t._traceLHS.begin(), t._traceLHS.end() );
    _traceRHS.insert( t._traceRHS.begin(), t._traceRHS.end() );
    _splitLHS.insert( t._splitLHS.begin(), t._splitLHS.end() );
    _splitRHS.insert( t._splitRHS.begin(), t._splitRHS.end() );
}

void XStmtTogether::insertVariable(const AstVariable* varPtr,
                                   const bool appearsOnLHS,
                                   const bool appearsOnRHS)
{
    // some XStmt types have a NULL left hand side (e.g. compound)
    if (varPtr)
    {
        // keep track of the precision of variables encountered
        if (varPtr->isFloat()) _prec32 = true;
        if (varPtr->isDouble()) _prec64 = true;

        // distinguish between kernel function arguments and private variables
        const bool isFunctionArgument = (0 == _lexicalDepth);

        if (varPtr->isTraceVariable())
        {
            const uint32_t varNum = varPtr->variable();

            if (0 == _traceVarDesc.count(varNum))
            {
                _traceVarDesc[varNum] = VariableDesc( varPtr,
                                                      isFunctionArgument );
            }

            if (_traceVarDesc[varNum].isArgument())
            {
                _traceArgs.insert(varNum);
            }

            if (appearsOnLHS)
            {
                _traceLHS.insert(varNum);
                _traceVarDesc[varNum].setAppearsOnLHS();
            }

            if (appearsOnRHS)
            {
                _traceRHS.insert(varNum);
                _traceVarDesc[varNum].setAppearsOnRHS();
            }
        }
        else
        {
            if (0 == _splitVarDesc.count(varPtr))
            {
                _splitVarDesc[varPtr] = VariableDesc( varPtr,
                                                      isFunctionArgument );
            }

            if (_splitVarDesc[varPtr].isArgument())
            {
                _splitArgs.insert(varPtr);
            }

            if (appearsOnLHS)
            {
                _splitLHS.insert(varPtr);
                _splitVarDesc[varPtr].setAppearsOnLHS();
            }

            if (appearsOnRHS)
            {
                _splitRHS.insert(varPtr);
                _splitVarDesc[varPtr].setAppearsOnRHS();
            }
        }
    }
}

void XStmtTogether::barrierVariable(AstVariable* lhsVar)
{
    if (lhsVar)
    {
        if (lhsVar->isTraceVariable())
        {
            _traceBarrier.insert(lhsVar->variable());
        }
        else
        {
            _splitBarrier.insert(lhsVar);
        }
    }
}

bool XStmtTogether::checkBarrier(const set< AstVariable* >& rhsVars,
                                 set< uint32_t >& traceMatch,
                                 set< AstVariable* >& splitMatch)
{
    bool needBarrier = false;

    // intersection of RHS with previous LHS variables
    for (set< AstVariable* >::const_iterator
         it = rhsVars.begin();
         it != rhsVars.end();
         it++)
    {
        if ((*it)->isTraceVariable())
        {
            if (_traceBarrier.count((*it)->variable()))
            {
                needBarrier = true;
                traceMatch.insert((*it)->variable());
            }
        }
        else
        {
            if (_splitBarrier.count(*it))
            {
                needBarrier = true;
                splitMatch.insert(*it);
            }
        }
    }

    // note: calling set erase() with begin and end iterator range crashes
    // that is why less elegant code with explicit for-loops is used instead

    // remove intersection trace variables
    for (set< uint32_t >::const_iterator
         it = traceMatch.begin();
         it != traceMatch.end();
         it++)
    {
        _traceBarrier.erase(*it);
    }

    // remove intersection split variables
    for (set< AstVariable* >::const_iterator
         it = splitMatch.begin();
         it != splitMatch.end();
         it++)
    {
        _splitBarrier.erase(*it);
    }

    return needBarrier;
}

XStmtTogether::XStmtTogether(void)
    : _togetherList(),
      _prec32(false),
      _prec64(false),
      _streamW(0),
      _streamH(0),
      _lexicalDepth(0),
      _prevIsExtension(false),
      _prevIsMatmul(false),
      _prevIsReduction(false),
      _specialK(NOTHING_SPECIAL),
      _reductionIsSpecial(true) { }

XStmtTogether::XStmtTogether(const XStmtTogether& other)
    : _togetherList(other._togetherList),
      _prec32(false),
      _prec64(false),
      _streamW(other._streamW),
      _streamH(other._streamH),
      _lexicalDepth(other._lexicalDepth),
      _prevIsExtension(other._prevIsExtension),
      _prevIsMatmul(other._prevIsMatmul),
      _prevIsReduction(other._prevIsReduction),
      _specialK(other._specialK),
      _reductionIsSpecial(other._reductionIsSpecial) { }

void XStmtTogether::clear(void)
{
    _togetherList.clear();

    // note: do not clear the trace and split variable descriptions
    // these accumulate for the trace over multiple kernels
    // however, each kernel keeps track of variables on LHS and RHS

    _traceArgs.clear();
    _splitArgs.clear();
    _traceBarrier.clear();
    _splitBarrier.clear();
    _traceLHS.clear();
    _traceRHS.clear();
    _splitLHS.clear();
    _splitRHS.clear();
    _prec32 = false;
    _prec64 = false;
    _streamW = 0;
    _streamH = 0;
    _lexicalDepth = 0;
    _prevIsExtension = false;
    _prevIsMatmul = false;
    _prevIsReduction = false;
    _specialK = NOTHING_SPECIAL;
}

const vector< XStmt* >& XStmtTogether::stuffInside(void) const
{
    return _togetherList;
}

const set< uint32_t >& XStmtTogether::traceArguments(void) const
{
    return _traceArgs;
}

const set< const AstVariable* >& XStmtTogether::splitArguments(void) const
{
    return _splitArgs;
}

const set< uint32_t >& XStmtTogether::traceLHS(void) const
{
    return _traceLHS;
}

const set< uint32_t >& XStmtTogether::traceRHS(void) const
{
    return _traceRHS;
}

const set< const AstVariable* >& XStmtTogether::splitLHS(void) const
{
    return _splitLHS;
}

const set< const AstVariable* >& XStmtTogether::splitRHS(void) const
{
    return _splitRHS;
}

const set< uint32_t >& XStmtTogether::traceTransposed(void) const
{
    return _traceTransposed;
}

const set< const AstVariable* >& XStmtTogether::splitTransposed(void) const
{
    return _splitTransposed;
}

const set< uint32_t >& XStmtTogether::traceGathered(void) const
{
    return _traceGathered;
}

const set< const AstVariable* >& XStmtTogether::splitGathered(void) const
{
    return _splitGathered;
}

bool XStmtTogether::isReadOnly(const uint32_t variable,
                               const bool appearsOnLHS,
                               const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isReadOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadOnly(const AstVariable* variable,
                               const bool appearsOnLHS,
                               const bool appearsOnRHS) const
{
    if (variable->isTraceVariable())
    {
        return isReadOnly(variable->variable(),
                          appearsOnLHS,
                          appearsOnRHS);
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isReadOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isWriteOnly(const uint32_t variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isWriteOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isWriteOnly(const AstVariable* variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (variable->isTraceVariable())
    {
        return isWriteOnly(variable->variable(),
                           appearsOnLHS,
                           appearsOnRHS);
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isWriteOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadWrite(const uint32_t variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isReadWrite(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadWrite(const AstVariable* variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (variable->isTraceVariable())
    {
        return isReadWrite(variable->variable(),
                           appearsOnLHS,
                           appearsOnRHS);
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isReadWrite(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadOnly(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isReadOnly(variable,
                          const_cast< XStmtTogether* >( this )
                              ->_traceVarDesc[ variable ]
                              .getAppearsOnLHS(),
                          const_cast< XStmtTogether* >( this )
                              ->_traceVarDesc[ variable ]
                              .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadOnly(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isReadOnly(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isReadOnly(variable,
                          const_cast< XStmtTogether* >( this )
                              ->_splitVarDesc[ variable ]
                              .getAppearsOnLHS(),
                          const_cast< XStmtTogether* >( this )
                              ->_splitVarDesc[ variable ]
                              .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isWriteOnly(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isWriteOnly(variable,
                           const_cast< XStmtTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< XStmtTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isWriteOnly(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isWriteOnly(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isWriteOnly(variable,
                           const_cast< XStmtTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< XStmtTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadWrite(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isReadWrite(variable,
                           const_cast< XStmtTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< XStmtTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isReadWrite(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isReadWrite(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isReadWrite(variable,
                           const_cast< XStmtTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< XStmtTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

size_t XStmtTogether::width(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .width();
    }
    else
    {
        return 0;
    }
}

size_t XStmtTogether::width(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return width(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .width();
    }
    else
    {
        return 0;
    }
}

size_t XStmtTogether::height(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .height();
    }
    else
    {
        return 0;
    }
}

size_t XStmtTogether::height(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return height(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .height();
    }
    else
    {
        return 0;
    }
}

size_t XStmtTogether::precision(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .precision();
    }
    else
    {
        return 0;
    }
}

size_t XStmtTogether::precision(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return precision(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< XStmtTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .precision();
    }
    else
    {
        return 0;
    }
}

bool XStmtTogether::isMixedPrecision(void) const
{
    return _prec32 && _prec64;
}

bool XStmtTogether::isSinglePrecision(void) const
{
    return _prec32;
}

bool XStmtTogether::isDoublePrecision(void) const
{
    return _prec64;
}

size_t XStmtTogether::streamW(void) const
{
    return _streamW;
}

size_t XStmtTogether::streamH(void) const
{
    return _streamH;
}

bool XStmtTogether::isPush(XStmt* s)
{
    const size_t beforeLength = _togetherList.size();
    s->accept(*this);
    const bool stmtAdded = beforeLength != _togetherList.size();

    if (stmtAdded)
    {
        _traceTransposed.insert(s->transposeTraceVars().begin(),
                                s->transposeTraceVars().end());

        _traceGathered.insert(s->gatherTraceVars().begin(),
                              s->gatherTraceVars().end());

        for (set< AstVariable* >::const_iterator
             it = s->transposeSplitVars().begin();
             it != s->transposeSplitVars().end();
             it++)
        {
            _splitTransposed.insert(*it);
        }

        for (set< AstVariable* >::const_iterator
             it = s->gatherSplitVars().begin();
             it != s->gatherSplitVars().end();
             it++)
        {
            _splitGathered.insert(*it);
        }
    }

    return stmtAdded;
}

XStmtTogether::SpecialKernel XStmtTogether::specialK(void) const
{
    return _specialK;
}

void XStmtTogether::reductionIsSpecial(const bool a)
{
    _reductionIsSpecial = a;
}

////////////////////////////////////////
// VisitXStmt

void XStmtTogether::visit(XStmtBarrier& s)
{
    // should never happen
}

void XStmtTogether::visit(XStmtCompound& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    _lexicalDepth++;

    // recurse into compound to find dimensions
    XStmtTogether tmpTogether(*this);
    for (vector< XStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(tmpTogether);
    }

    _lexicalDepth--;

    boundIndex(tmpTogether);
    pushList(s, tmpTogether);
}

void XStmtTogether::visit(XStmtCreateData& s)
{
    if (isSpecial()) return;

    if (0 == _lexicalDepth)
    {
        // outermost lexical scope variable may span kernels or swizzle back
        if (! isCode()) pushList(s);
    }
    else
    {
        // inside a for-loop, variable must be private
        boundIndex(s);
        pushList(s);
    }
}

void XStmtTogether::visit(XStmtExtension& s)
{
    if (isSpecial()) return;
    if (isEmpty())
    {
        boundIndex(s);
        pushList(s);

        _prevIsExtension = true;
        _specialK = SPECIAL_EXTENSION;
    }
}

void XStmtTogether::visit(XStmtExtensionAuto& s)
{
    if (isSpecial()) return;
    if (isEmpty())
    {
        boundIndex(s.extensionWidth(), s.extensionHeight());
        pushList(s);

        _prevIsExtension = true;
        _specialK = SPECIAL_EXTENSION_AUTO;
    }
}

void XStmtTogether::visit(XStmtIdSpace& s)
{
    // should never happen
}

void XStmtTogether::visit(XStmtIndex& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtLiteral& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtMatmul& s)
{
    if (isSpecial()) return;
    if (isEmpty())
    {
        boundIndex(s);
        pushList(s);

        _prevIsMatmul = true;
        _specialK = SPECIAL_MATMUL;
    }
}

void XStmtTogether::visit(XStmtMatmulAuto& s)
{
    if (isSpecial()) return;
    if (isEmpty())
    {
        boundIndex(s.widthC(), s.heightC());
        pushList(s);

        _prevIsMatmul = true;
        _specialK = SPECIAL_MATMUL_AUTO;
    }
}

void XStmtTogether::visit(XStmtReadData& s)
{
    if (isSpecial()) return;
    if (isEmpty())
    {
        pushList(s);
    }
    else
    {
        if (isCode())
        {
            setIndex(s);
        }
    }
}

void XStmtTogether::visit(XStmtReduce& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    boundIndex(s);
    pushList(s);

    // reductions force kernel boundary
    _prevIsReduction = true;
}

void XStmtTogether::visit(XStmtRepeat& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    _lexicalDepth++;

    // recurse into compound to find dimensions
    XStmtTogether tmpTogether(*this);
    s.stuffInside()->accept(tmpTogether);

    _lexicalDepth--;

    boundIndex(tmpTogether);
    pushList(s, tmpTogether);
}

void XStmtTogether::visit(XStmtRNGrand& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtRNGseed& s)
{
    if (isSpecial()) return;
    if (! isCode()) pushList(s);
}

void XStmtTogether::visit(XStmtSendData& s)
{
    if (isSpecial()) return;
    if (! isCode()) pushList(s);
}

void XStmtTogether::visit(XStmtSingle& s)
{
    if (isSpecial()) return;
    if (isPragmas()) return;

    boundIndex(s);
    pushList(s);
}

}; // namespace chai_internal
