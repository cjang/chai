// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtExtensionAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

StmtExtensionAuto::StmtExtensionAuto(AstVariable* lhs,
                                     AstExtension* rhs)
    : Stmt(),
      _lhsVariable(lhs),
      _extObj(rhs)
{
    buoyancySurface();
}

string StmtExtensionAuto::extensionName(void) const
{
    return _extObj->extensionName();
}

bool StmtExtensionAuto::extensionEnqueue(MemManager& memMgr,
                                         VectorTrace& vt)
{
    return _extObj->extensionEnqueueAuto(memMgr, vt, _lhsVariable);
}

size_t StmtExtensionAuto::extensionWidth(void) const
{
    return _extObj->W();
}

size_t StmtExtensionAuto::extensionHeight(void) const
{
    return _extObj->H();
}

bool StmtExtensionAuto::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtExtensionAuto::accept(VisitStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
