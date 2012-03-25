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
#include "StmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// single AST statement

void StmtSingle::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

StmtSingle::StmtSingle(AstVariable* lhs)
    : _scalarToScalar(false)
{
    // set LHS variable
    lhsVariable(lhs);

    // set RHS variables and buoyancy
    buoyancySink(); // default is sink towards the readout
    descendAst(*lhs);
}

bool StmtSingle::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtSingle::accept(VisitStmt& v)
{
    v.visit(*this);
}

size_t StmtSingle::W(void) const
{
    return lhsVariable()->W();
}

size_t StmtSingle::H(void) const
{
    return lhsVariable()->H();
}

size_t StmtSingle::precision(void) const
{
    return lhsVariable()->precision();
}

void StmtSingle::scalarToScalar(const bool v)
{
    _scalarToScalar = v;
}

bool StmtSingle::scalarToScalar(void) const
{
    return _scalarToScalar;
}

////////////////////////////////////////
// VisitAst

void StmtSingle::visit(AstAccum& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstArrayMem& v)
{
    buoyancyRise();
}

void StmtSingle::visit(AstCond& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstConvert& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstDotprod& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstExtension& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstFun1& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstFun2& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstFun3& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstGather& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstIdxdata& v)
{
}

void StmtSingle::visit(AstLitdata& v)
{
}

void StmtSingle::visit(AstMakedata& v)
{
    buoyancyRise();
}

void StmtSingle::visit(AstMatmulMM& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void StmtSingle::visit(AstMatmulMV& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void StmtSingle::visit(AstMatmulVM& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void StmtSingle::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstReadout& v)
{
}

void StmtSingle::visit(AstRNGnormal& v)
{
}

void StmtSingle::visit(AstRNGuniform& v)
{
}

void StmtSingle::visit(AstScalar& v)
{
}

void StmtSingle::visit(AstTranspose& v)
{
    descendAst(v);
}

void StmtSingle::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
