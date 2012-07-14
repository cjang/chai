// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// represents a LHS/RHS variable

AstVariable::AstVariable(AstArrayMem* barg)
    : BaseAst(barg->prec(),
              barg->W(),
              barg->H(),
              barg->slots(),
              barg->randomness()),
      _isTraceVariable(false),
      _isLiveVariable(false),
      _variable(-1),
      _version(-1),
      _frontMem(barg->frontMem()),
      _kindOfVariable(SPLIT_ARRAY_MEMORY),
      _disableDotHack(false)
{
    pushArg(barg);
}

AstVariable::AstVariable(BaseAst* barg,
                         const int dummy)
    : BaseAst(barg->prec(),
              barg->W(),
              barg->H(),
              barg->slots(),
              barg->randomness()),
      _isTraceVariable(false),
      _isLiveVariable(false),
      _variable(-1),
      _version(-1),
      _frontMem(),
      _kindOfVariable(SPLIT_OPERATION),
      _disableDotHack(false)
{
    pushArg(barg);
}

AstVariable::AstVariable(BaseAst* barg,
                         const uint32_t variable,
                         const uint32_t version,
                         const bool isLive,
                         const vector< FrontMem* >& frontMem)
    : BaseAst(barg->prec(),
              barg->W(),
              barg->H(),
              barg->slots(),
              barg->randomness()),
      _isTraceVariable(true),
      _isLiveVariable(isLive),
      _variable(variable),
      _version(version),
      _frontMem(frontMem),
      _kindOfVariable(TRACE_VARIABLE),
      _disableDotHack(false)
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

void AstVariable::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
