// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// represents a LHS/RHS variable

AstVariable::AstVariable(BaseAst* barg)
    : BaseAst(barg->W(), barg->H(), barg->isDouble()),
      _isTraceVariable(false),
      _isLiveVariable(false),
      _variable(-1),
      _version(-1),
      _accessMode(VAR_IS_READWRITE)
{
    pushArg(barg);
}

AstVariable::AstVariable(BaseAst* barg,
                         const uint32_t variable,
                         const uint32_t version,
                         const bool isLive)
    : BaseAst(barg->W(), barg->H(), barg->isDouble()),
      _isTraceVariable(true),
      _isLiveVariable(isLive),
      _variable(variable),
      _version(version),
      _accessMode(VAR_IS_READWRITE)
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

bool AstVariable::isReadOnly(void) const
{
    return VAR_IS_READONLY == _accessMode;
}

bool AstVariable::isWriteOnly(void) const
{
    return VAR_IS_WRITEONLY == _accessMode;
}

void AstVariable::setReadOnly(void)
{
    _accessMode = VAR_IS_READONLY;
}

void AstVariable::setWriteOnly(void)
{
    _accessMode = VAR_IS_WRITEONLY;
}

void AstVariable::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
