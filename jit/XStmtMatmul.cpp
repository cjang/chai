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
#include "XStmtMatmul.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matrix multiplication
// (all cases except outer product)

void XStmtMatmul::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

XStmtMatmul::XStmtMatmul(AstVariable* lhs,
                         AstMatmulMM* rhs,
                         const bool convert)
    : _rhsMatmulMM(rhs),
      _rhsMatmulMV(NULL),
      _rhsMatmulVM(NULL),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

XStmtMatmul::XStmtMatmul(AstVariable* lhs,
                         AstMatmulMV* rhs,
                         const bool convert)
    : _rhsMatmulMM(NULL),
      _rhsMatmulMV(rhs),
      _rhsMatmulVM(NULL),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

XStmtMatmul::XStmtMatmul(AstVariable* lhs,
                         AstMatmulVM* rhs,
                         const bool convert)
    : _rhsMatmulMM(NULL),
      _rhsMatmulMV(NULL),
      _rhsMatmulVM(rhs),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

bool XStmtMatmul::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtMatmul::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstMatmulMM* XStmtMatmul::matmulPtr(void) const
{
    return _rhsMatmulMM;
}

AstMatmulMV* XStmtMatmul::matvecPtr(void) const
{
    return _rhsMatmulMV;
}

AstMatmulVM* XStmtMatmul::vecmatPtr(void) const
{
    return _rhsMatmulVM;
}

bool XStmtMatmul::isConvert(void) const
{
    return _isConvert;
}

////////////////////////////////////////
// VisitAst

void XStmtMatmul::visit(AstAccum& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstArrayMem& v)
{
}

void XStmtMatmul::visit(AstBinop& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstCond& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstConvert& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstDotprod& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstGather& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstIdxdata& v)
{
}

void XStmtMatmul::visit(AstIsomorph& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstLitdata& v)
{
}

void XStmtMatmul::visit(AstMakedata& v)
{
}

void XStmtMatmul::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void XStmtMatmul::visit(AstReadout& v)
{
}

void XStmtMatmul::visit(AstRNGnormal& v)
{
}

void XStmtMatmul::visit(AstRNGuniform& v)
{
}

void XStmtMatmul::visit(AstScalar& v)
{
}

void XStmtMatmul::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
