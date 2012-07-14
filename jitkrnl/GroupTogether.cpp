// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "GroupTogether.hpp"
#include "OCLhacks.hpp"
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

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// variable description

GroupTogether::VariableDesc::VariableDesc(void)
    : _variableObj(NULL),
      _isArgument(false),
      _appearsOnLHS(false),
      _appearsOnRHS(false) { }

GroupTogether::VariableDesc::VariableDesc(const AstVariable* obj,
                                          const bool isArg)
    : _variableObj(obj),
      _isArgument(isArg),
      _appearsOnLHS(false),
      _appearsOnRHS(false) { }

GroupTogether::VariableDesc::VariableDesc(const VariableDesc& other)
    : _variableObj(other._variableObj),
      _isArgument(other._isArgument),
      _appearsOnLHS(other._appearsOnLHS),
      _appearsOnRHS(other._appearsOnRHS) { }

GroupTogether::VariableDesc& GroupTogether::VariableDesc::operator= (
                                 const VariableDesc& other )
{
    _variableObj = other._variableObj;
    _isArgument = other._isArgument;
    _appearsOnLHS = other._appearsOnLHS;
    _appearsOnRHS = other._appearsOnRHS;

    return *this;
}

bool GroupTogether::VariableDesc::isArgument(void) const
{
    return _isArgument;
}

bool GroupTogether::VariableDesc::isReadOnly(const bool appearsOnLHS,
                                             const bool appearsOnRHS) const
{
    return _variableObj->isReadOnly(appearsOnLHS, appearsOnRHS);
}

bool GroupTogether::VariableDesc::isWriteOnly(const bool appearsOnLHS,
                                              const bool appearsOnRHS) const
{
    return _variableObj->isWriteOnly(appearsOnLHS, appearsOnRHS);
}

bool GroupTogether::VariableDesc::isReadWrite(const bool appearsOnLHS,
                                              const bool appearsOnRHS) const
{
    return _variableObj->isReadWrite(appearsOnLHS, appearsOnRHS);
}

bool GroupTogether::VariableDesc::getAppearsOnLHS(void) const
{
    return _appearsOnLHS;
}

bool GroupTogether::VariableDesc::getAppearsOnRHS(void) const
{
    return _appearsOnRHS;
}

void GroupTogether::VariableDesc::setAppearsOnLHS(void)
{
    _appearsOnLHS = true;
}

void GroupTogether::VariableDesc::setAppearsOnRHS(void)
{
    _appearsOnRHS = true;
}

bool GroupTogether::VariableDesc::isReadOnly(void) const
{
    return isReadOnly(_appearsOnLHS, _appearsOnRHS);
}

bool GroupTogether::VariableDesc::isWriteOnly(void) const
{
    return isWriteOnly(_appearsOnLHS, _appearsOnRHS);
}

bool GroupTogether::VariableDesc::isReadWrite(void) const
{
    return isReadWrite(_appearsOnLHS, _appearsOnRHS);
}

size_t GroupTogether::VariableDesc::width(void) const
{
    return _variableObj->W();
}

size_t GroupTogether::VariableDesc::height(void) const
{
    return _variableObj->H();
}

size_t GroupTogether::VariableDesc::prec(void) const
{
    return _variableObj->prec();
}

////////////////////////////////////////
// data movement and code statement
// grouping

bool GroupTogether::isExtension(void)
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

bool GroupTogether::isMatmul(void)
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

bool GroupTogether::isOpenCL(void)
{
    if (_prevIsOpenCL)
    {
        _prevIsOpenCL = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReduction(void)
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

bool GroupTogether::isSpecial(void)
{
    if (_reductionIsSpecial)
    {
        return isExtension() || isMatmul() || isOpenCL() || isReduction();
    }
    else
    {
        return isExtension() || isMatmul() || isOpenCL();
    }
}

bool GroupTogether::isEmpty(void) const
{
    return _togetherList.empty();
}

bool GroupTogether::isMetaKernel(void) const
{
    // check for non-empty list with no dimensions yet
    return 0 == _streamW && 0 == _streamH && !_togetherList.empty();
}

bool GroupTogether::isCode(void) const
{
    // check for dimensions which indicates there is code
    return 0 != _streamW || 0 != _streamH;
}

void GroupTogether::boundIndex(const size_t w, const size_t h)
{
    if (w > _streamW) _streamW = w;
    if (h > _streamH) _streamH = h;
}

void GroupTogether::boundIndex(const GroupTogether& t)
{
    boundIndex(t.streamW(), t.streamH());
}

void GroupTogether::boundIndex(const Stmt& s)
{
    return boundIndex(s.lhsVariable()->W(), s.lhsVariable()->H());
}

void GroupTogether::setIndex(const StmtReadData& s)
{
    _streamW = s.lhsVariable()->W();
    _streamH = s.lhsVariable()->H();
}

void GroupTogether::setIndex(const StmtReduce& s)
{
    _streamW = s.lhsVariable()->W();
    _streamH = s.lhsVariable()->H();
}

void GroupTogether::pushList(Stmt& s)
{
    // insert barrier if RHS references earlier LHS
    set< uint32_t >           traceMatch;
    set< const AstVariable* > splitMatch;
    if ( checkBarrier(s.rhsVariable(),
                      traceMatch,
                      splitMatch) )
    {
        _togetherList.push_back( new StmtBarrier(traceMatch,
                                                 splitMatch) );
    }

    // add current statement
    _togetherList.push_back(&s);

    // RHS variables
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin(); it != s.rhsVariable().end(); it++)
    {
        insertVariable(*it, false, true);
    }

    // LHS variable
    insertVariable(s.lhsVariable(), s.trackLHS(), false);

    barrierVariable(s.lhsVariable());
}

// used by compound and repeat statements
void GroupTogether::pushList(Stmt& s, GroupTogether& t)
{
    _togetherList.push_back(&s);

    for (map< uint32_t, VariableDesc >::iterator
         it = t._traceVarDesc.begin(); it != t._traceVarDesc.end(); it++)
    {
        const uint32_t varNum = (*it).first;
        VariableDesc& varDesc = (*it).second;

        if (varDesc.getAppearsOnLHS()) _traceVarDesc[varNum].setAppearsOnLHS();
        if (varDesc.getAppearsOnRHS()) _traceVarDesc[varNum].setAppearsOnRHS();
    }

    for (map< const AstVariable*, VariableDesc >::iterator
         it = t._splitVarDesc.begin(); it != t._splitVarDesc.end(); it++)
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

void GroupTogether::insertVariable(const AstVariable* varPtr,
                                   const bool appearsOnLHS,
                                   const bool appearsOnRHS)
{
    // some Stmt types have a NULL left hand side (e.g. compound)
    if (varPtr)
    {
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

void GroupTogether::barrierVariable(AstVariable* lhsVar)
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

bool GroupTogether::checkBarrier(const set< AstVariable* >& rhsVars,
                                 set< uint32_t >& traceMatch,
                                 set< const AstVariable* >& splitMatch)
{
    bool needBarrier = false;

    // intersection of RHS with previous LHS variables
    for (set< AstVariable* >::const_iterator
         it = rhsVars.begin(); it != rhsVars.end(); it++)
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
         it = traceMatch.begin(); it != traceMatch.end(); it++)
    {
        _traceBarrier.erase(*it);
    }

    // remove intersection split variables
    for (set< const AstVariable* >::const_iterator
         it = splitMatch.begin(); it != splitMatch.end(); it++)
    {
        _splitBarrier.erase(*it);
    }

    return needBarrier;
}

GroupTogether::GroupTogether(void)
    : _togetherList(),
      _scalarVecLength(false),
      _constructorLHS(),
      _destructorLHS(),
      _streamW(0),
      _streamH(0),
      _lexicalDepth(0),
      _prevIsExtension(false),
      _prevIsMatmul(false),
      _prevIsOpenCL(false),
      _prevIsReduction(false),
      _specialK(NOTHING_SPECIAL),
      _reductionIsSpecial(true) { }

GroupTogether::GroupTogether(const GroupTogether& other)
    : _togetherList(other._togetherList),
      _scalarVecLength(other._scalarVecLength),
      _constructorLHS(other._constructorLHS),
      _destructorLHS(other._destructorLHS),
      _streamW(other._streamW),
      _streamH(other._streamH),
      _lexicalDepth(other._lexicalDepth),
      _prevIsExtension(other._prevIsExtension),
      _prevIsMatmul(other._prevIsMatmul),
      _prevIsOpenCL(other._prevIsOpenCL),
      _prevIsReduction(other._prevIsReduction),
      _specialK(other._specialK),
      _reductionIsSpecial(other._reductionIsSpecial) { }

void GroupTogether::clear(void)
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
    _scalarVecLength = false;
    _constructorLHS.clear();
    _destructorLHS.clear();
    _streamW = 0;
    _streamH = 0;
    _lexicalDepth = 0;
    _prevIsExtension = false;
    _prevIsMatmul = false;
    _prevIsOpenCL = false;
    _prevIsReduction = false;
    _specialK = NOTHING_SPECIAL;
}

const vector< Stmt* >& GroupTogether::stuffInside(void) const
{
    return _togetherList;
}

const set< uint32_t >& GroupTogether::traceArguments(void) const
{
    return _traceArgs;
}

const set< const AstVariable* >& GroupTogether::splitArguments(void) const
{
    return _splitArgs;
}

const set< uint32_t >& GroupTogether::traceLHS(void) const
{
    return _traceLHS;
}

const set< uint32_t >& GroupTogether::traceRHS(void) const
{
    return _traceRHS;
}

const set< const AstVariable* >& GroupTogether::splitLHS(void) const
{
    return _splitLHS;
}

const set< const AstVariable* >& GroupTogether::splitRHS(void) const
{
    return _splitRHS;
}

bool GroupTogether::scalarVecLength(void) const
{
    return _scalarVecLength;
}

bool GroupTogether::isReadOnly(const uint32_t variable,
                               const bool appearsOnLHS,
                               const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isReadOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadOnly(const AstVariable* variable,
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
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isReadOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isWriteOnly(const uint32_t variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isWriteOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isWriteOnly(const AstVariable* variable,
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
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isWriteOnly(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadWrite(const uint32_t variable,
                                const bool appearsOnLHS,
                                const bool appearsOnRHS) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .isReadWrite(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadWrite(const AstVariable* variable,
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
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .isReadWrite(appearsOnLHS, appearsOnRHS);
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadOnly(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isReadOnly(variable,
                          const_cast< GroupTogether* >( this )
                              ->_traceVarDesc[ variable ]
                              .getAppearsOnLHS(),
                          const_cast< GroupTogether* >( this )
                              ->_traceVarDesc[ variable ]
                              .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadOnly(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isReadOnly(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isReadOnly(variable,
                          const_cast< GroupTogether* >( this )
                              ->_splitVarDesc[ variable ]
                              .getAppearsOnLHS(),
                          const_cast< GroupTogether* >( this )
                              ->_splitVarDesc[ variable ]
                              .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isWriteOnly(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isWriteOnly(variable,
                           const_cast< GroupTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< GroupTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isWriteOnly(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isWriteOnly(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isWriteOnly(variable,
                           const_cast< GroupTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< GroupTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadWrite(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return isReadWrite(variable,
                           const_cast< GroupTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< GroupTogether* >( this )
                               ->_traceVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

bool GroupTogether::isReadWrite(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return isReadWrite(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return isReadWrite(variable,
                           const_cast< GroupTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnLHS(),
                           const_cast< GroupTogether* >( this )
                               ->_splitVarDesc[ variable ]
                               .getAppearsOnRHS());
    }
    else
    {
        return false;
    }
}

size_t GroupTogether::width(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .width();
    }
    else
    {
        return 0;
    }
}

size_t GroupTogether::width(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return width(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .width();
    }
    else
    {
        return 0;
    }
}

size_t GroupTogether::height(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .height();
    }
    else
    {
        return 0;
    }
}

size_t GroupTogether::height(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return height(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .height();
    }
    else
    {
        return 0;
    }
}

size_t GroupTogether::prec(const uint32_t variable) const
{
    if (_traceVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_traceVarDesc[ variable ]
                   .prec();
    }
    else
    {
        return 0;
    }
}

size_t GroupTogether::prec(const AstVariable* variable) const
{
    if (variable->isTraceVariable())
    {
        return prec(variable->variable());
    }

    if (_splitVarDesc.count(variable))
    {
        return const_cast< GroupTogether* >( this )
                   ->_splitVarDesc[ variable ]
                   .prec();
    }
    else
    {
        return 0;
    }
}

set< uint32_t > GroupTogether::internalVars(void) const
{
    set< uint32_t > s;

    for (set< uint32_t >::const_iterator
         it = _constructorLHS.begin(); it != _constructorLHS.end(); it++)
    {
        if (_destructorLHS.count(*it))
            s.insert(*it);
    }

    return s;
}

size_t GroupTogether::streamW(void) const
{
    return _streamW;
}

size_t GroupTogether::streamH(void) const
{
    return _streamH;
}

bool GroupTogether::isPush(Stmt* s)
{
    const size_t beforeLength = _togetherList.size();
    s->accept(*this);
    const bool stmtAdded = beforeLength != _togetherList.size();

    if (s->lhsVariable())
    {
        if (s->getConstructorLHS())
            _constructorLHS.insert(s->lhsVariable()->variable());

        if (s->getDestructorLHS())
            _destructorLHS.insert(s->lhsVariable()->variable());
    }

    if (stmtAdded)
    {
        if (s->scalarVecLength())
            _scalarVecLength = true;
    }

    return stmtAdded;
}

GroupTogether::SpecialKernel GroupTogether::specialK(void) const
{
    return _specialK;
}

void GroupTogether::reductionIsSpecial(const bool a)
{
    _reductionIsSpecial = a;
}

////////////////////////////////////////
// VisitStmt

void GroupTogether::visit(StmtBarrier& s)
{
    // should never happen
}

void GroupTogether::visit(StmtCompound& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    _lexicalDepth++;

    // recurse into compound to find dimensions
    GroupTogether tmpTogether(*this);
    for (vector< Stmt* >::const_iterator
         it = s.stuffInside().begin(); it != s.stuffInside().end(); it++)
    {
        (*it)->accept(tmpTogether);

        if ((*it)->lhsVariable())
        {
            if ((*it)->getConstructorLHS())
                _constructorLHS.insert((*it)->lhsVariable()->variable());

            if ((*it)->getDestructorLHS())
                _destructorLHS.insert((*it)->lhsVariable()->variable());
        }
    }

    _lexicalDepth--;

    boundIndex(tmpTogether);
    pushList(s, tmpTogether);
}

void GroupTogether::visit(StmtCreateData& s)
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

void GroupTogether::visit(StmtExtension& s)
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

void GroupTogether::visit(StmtExtensionAuto& s)
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

void GroupTogether::visit(StmtGatherAuto& s)
{
    if (isSpecial()) return;

    if (! isCode()) pushList(s);
}

void GroupTogether::visit(StmtIdSpace& s)
{
    // should never happen
}

void GroupTogether::visit(StmtIndex& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    boundIndex(s);
    pushList(s);
}

void GroupTogether::visit(StmtLiteral& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    boundIndex(s);
    pushList(s);
}

void GroupTogether::visit(StmtMatmul& s)
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

void GroupTogether::visit(StmtMatmulAuto& s)
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

void GroupTogether::visit(StmtOpenCL& s)
{
    if (isSpecial()) return;

    if (isEmpty())
    {
        boundIndex(0, 0);
        pushList(s);

        _prevIsOpenCL = true;
        _specialK = SPECIAL_OPENCL;
    }
}

void GroupTogether::visit(StmtReadData& s)
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

void GroupTogether::visit(StmtReduce& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    setIndex(s);
    pushList(s);

    // reductions force kernel boundary
    _prevIsReduction = true;
}

void GroupTogether::visit(StmtRepeat& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    _lexicalDepth++;

    // recurse into compound to find dimensions
    GroupTogether tmpTogether(*this);
    s.stuffInside()->accept(tmpTogether);

    _lexicalDepth--;

    boundIndex(tmpTogether);
    pushList(s, tmpTogether);
}

void GroupTogether::visit(StmtSendData& s)
{
    if (isSpecial()) return;

    if (! isCode()) pushList(s);
}

void GroupTogether::visit(StmtSingle& s)
{
    if (isSpecial()) return;
    if (isMetaKernel()) return;

    boundIndex(s);
    pushList(s);
}

}; // namespace chai_internal
