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
#include "AstOpenCL.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "MemManager.hpp"
#include "StmtExtension.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

void StmtExtension::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

StmtExtension::StmtExtension(AstVariable* lhs,
                             AstExtension* rhs)
    : Stmt(),
      _extObj(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around this one
    buoyancyNeutral();
    descendAst(*lhs);
}

StmtExtension::~StmtExtension(void) { }

string StmtExtension::extensionName(void) const
{
    return _extObj->extensionName();
}

bool StmtExtension::extensionEnqueue(MemManager& memMgr,
                                     VectorTrace& vt)
{
    return _extObj->extensionEnqueue(memMgr, vt, lhsVariable());
}

bool StmtExtension::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtExtension::accept(VisitStmt& v)
{
    v.visit(*this);
}

////////////////////////////////////////
// VisitAst

void StmtExtension::visit(AstAccum& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstArrayMem& v)
{
}

void StmtExtension::visit(AstCond& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstConvert& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstDotprod& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstExtension& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstFun1& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstFun2& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstFun3& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstGather& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstIdxdata& v)
{
}

void StmtExtension::visit(AstLitdata& v)
{
}

void StmtExtension::visit(AstMakedata& v)
{
}

void StmtExtension::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstOpenCL& v)
{
}

void StmtExtension::visit(AstReadout& v)
{
}

void StmtExtension::visit(AstRNGnormal& v)
{
}

void StmtExtension::visit(AstRNGuniform& v)
{
}

void StmtExtension::visit(AstScalar& v)
{
}

void StmtExtension::visit(AstTranspose& v)
{
    descendAst(v);
}

void StmtExtension::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
