// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstBinop.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstIsomorph.hpp"
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
#include "AstVariable.hpp"
#include "XStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// single AST statement

void XStmtSingle::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

XStmtSingle::XStmtSingle(AstVariable* lhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // set RHS variables and buoyancy
    buoyancySink(); // default is sink towards the readout
    descendAst(*lhs);
}

bool XStmtSingle::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtSingle::accept(VisitXStmt& v)
{
    v.visit(*this);
}

size_t XStmtSingle::W(void) const
{
    return lhsVariable()->W();
}

size_t XStmtSingle::H(void) const
{
    return lhsVariable()->H();
}

size_t XStmtSingle::precision(void) const
{
    return lhsVariable()->isDouble()
               ? sizeof(double)
               : sizeof(float);
}

////////////////////////////////////////
// VisitAst

void XStmtSingle::visit(AstAccum& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstArrayMem& v)
{
    buoyancyRise();
}

void XStmtSingle::visit(AstBinop& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstCond& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstConvert& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstDotprod& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstGather& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstIdxdata& v)
{
}

void XStmtSingle::visit(AstIsomorph& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstLitdata& v)
{
}

void XStmtSingle::visit(AstMakedata& v)
{
    buoyancyRise();
}

void XStmtSingle::visit(AstMatmulMM& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void XStmtSingle::visit(AstMatmulMV& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void XStmtSingle::visit(AstMatmulVM& v)
{
    descendAst(v);
    buoyancyNeutral();
}

void XStmtSingle::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void XStmtSingle::visit(AstReadout& v)
{
}

void XStmtSingle::visit(AstRNGnormal& v)
{
}

void XStmtSingle::visit(AstRNGuniform& v)
{
}

void XStmtSingle::visit(AstScalar& v)
{
}

void XStmtSingle::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
