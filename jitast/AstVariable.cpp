// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"
#include "MemManager.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// represents a LHS/RHS variable

AstVariable::AstVariable(AstArrayMem* barg)
    : BaseAst(barg->W(), barg->H(), barg->precision()),
      _isTraceVariable(false),
      _isLiveVariable(false),
      _variable(-1),
      _version(-1),
      _frontMem(barg->frontMem()),
      _kindOfVariable(SPLIT_ARRAY_MEMORY),
      _sameDataAcrossTraces(barg->sameDataAcrossTraces()),
      _disableDotHack(false)
/*FIXME - remove this,
      _valueFromRNG(false)
*/
/*FIXME - remove this,
      _forceWriteback(false)
*/
{
    pushArg(barg);
}

AstVariable::AstVariable(BaseAst* barg,
                         const bool writable)
    : BaseAst(barg->W(), barg->H(), barg->precision()),
      _isTraceVariable(false),
      _isLiveVariable(false),
      _variable(-1),
      _version(-1),
      _frontMem(),
      _kindOfVariable(SPLIT_OPERATION),
      _sameDataAcrossTraces(false),
      _disableDotHack(false)
/*FIXME - remove this,
      _valueFromRNG(false)
*/
/*FIXME - remove this,
      _forceWriteback(false)
*/
{
    pushArg(barg);
}

AstVariable::AstVariable(BaseAst* barg,
                         const uint32_t variable,
                         const uint32_t version,
                         const bool isLive,
                         const vector< FrontMem* >& frontMem)
    : BaseAst(barg->W(), barg->H(), barg->precision()),
      _isTraceVariable(true),
      _isLiveVariable(isLive),
      _variable(variable),
      _version(version),
      _frontMem(frontMem),
      _kindOfVariable(TRACE_VARIABLE),
      _sameDataAcrossTraces(MemManager::checkSameDataAcrossTraces(frontMem)),
      _disableDotHack(false)
/*FIXME - remove this,
      _valueFromRNG(false)
*/
/*FIXME - remove this,
      _forceWriteback(false)
*/
{
    pushArg(barg);
}

bool AstVariable::isTraceVariable(void) const
{
    return _isTraceVariable;
}

bool AstVariable::isLiveVariable(void) const
{
    return _isLiveVariable;
}

uint32_t AstVariable::variable(void) const
{
    return _variable;
}

uint32_t AstVariable::version(void) const
{
    return _version;
}

const vector< FrontMem* >& AstVariable::frontMem(void) const
{
    return _frontMem;
}

bool AstVariable::isSameDataAcrossTraces(void) const
{
    return _sameDataAcrossTraces;
}

bool AstVariable::isReadOnly(const bool appearsOnLHS,
                             const bool appearsOnRHS) const
{
    switch (_kindOfVariable)
    {
        case (SPLIT_ARRAY_MEMORY) : return true;
        case (SPLIT_OPERATION) : return false;
        case (TRACE_VARIABLE) : return ! appearsOnLHS && appearsOnRHS;
    }
}

bool AstVariable::isWriteOnly(const bool appearsOnLHS,
                              const bool appearsOnRHS) const
{
    switch (_kindOfVariable)
    {
        case (SPLIT_ARRAY_MEMORY) : return false;
        case (SPLIT_OPERATION) : return false;
        case (TRACE_VARIABLE) : return appearsOnLHS && ! appearsOnRHS;
    }
}

bool AstVariable::isReadWrite(const bool appearsOnLHS,
                              const bool appearsOnRHS) const
{
    switch (_kindOfVariable)
    {
        case (SPLIT_ARRAY_MEMORY) : return false;
        case (SPLIT_OPERATION) : return true;
        case (TRACE_VARIABLE) : return appearsOnLHS && appearsOnRHS;
    }
}

bool AstVariable::enableDotHack(void) const
{
    return ! _disableDotHack;
}

void AstVariable::disableDotHack(void)
{
    _disableDotHack = true;
}

/*FIXME - remove this
bool AstVariable::getValueFromRNG(void) const
{
    return _valueFromRNG;
}

void AstVariable::setValueFromRNG(void)
{
    _valueFromRNG = true;
}
*/

/*FIXME - remove this
bool AstVariable::getForceWriteback(void) const
{
    return _forceWriteback;
}

void AstVariable::setForceWriteback(void)
{
    _forceWriteback = true;
}
*/

void AstVariable::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
