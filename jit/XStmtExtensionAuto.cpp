// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtExtensionAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

XStmtExtensionAuto::XStmtExtensionAuto(AstVariable* lhs,
                                       AstExtension* rhs)
    : XStmt(),
      _lhsVariable(lhs),
      _extObj(rhs)
{
    buoyancySurface();
}

string XStmtExtensionAuto::extensionName(void) const
{
    return _extObj->extensionName();
}

bool XStmtExtensionAuto::extensionEnqueue(MemManager& memMgr,
                                          VectorTrace& vt)
{
    return _extObj->extensionEnqueueAuto(memMgr, vt, _lhsVariable);
}

size_t XStmtExtensionAuto::extensionWidth(void) const
{
    return _extObj->W();
}

size_t XStmtExtensionAuto::extensionHeight(void) const
{
    return _extObj->H();
}

bool XStmtExtensionAuto::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtExtensionAuto::accept(VisitXStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
