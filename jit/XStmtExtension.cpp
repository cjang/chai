// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "MemManager.hpp"
#include "XStmtExtension.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

void XStmtExtension::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

XStmtExtension::XStmtExtension(AstVariable* lhs,
                               AstExtension* rhs)
    : XStmt(),
      _extObj(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around this one
    buoyancyNeutral();
    descendAst(*lhs);
}

XStmtExtension::~XStmtExtension(void) { }

string XStmtExtension::extensionName(void) const
{
    return _extObj->extensionName();
}

bool XStmtExtension::extensionEnqueue(MemManager& memMgr,
                                      VectorTrace& vt)
{
    return _extObj->extensionEnqueue(memMgr, vt, lhsVariable());
}

bool XStmtExtension::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtExtension::accept(VisitXStmt& v)
{
    v.visit(*this);
}

////////////////////////////////////////
// VisitAst

void XStmtExtension::visit(AstAccum& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstArrayMem& v)
{
}

void XStmtExtension::visit(AstCond& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstConvert& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstDotprod& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstExtension& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstFun1& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstFun2& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstFun3& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstGather& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstIdxdata& v)
{
}

void XStmtExtension::visit(AstLitdata& v)
{
}

void XStmtExtension::visit(AstMakedata& v)
{
}

void XStmtExtension::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstReadout& v)
{
}

void XStmtExtension::visit(AstRNGnormal& v)
{
}

void XStmtExtension::visit(AstRNGuniform& v)
{
}

void XStmtExtension::visit(AstScalar& v)
{
}

void XStmtExtension::visit(AstTranspose& v)
{
    descendAst(v);
}

void XStmtExtension::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
