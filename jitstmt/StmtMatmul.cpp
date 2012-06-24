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
#include "StmtMatmul.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matrix multiplication
// (all cases except outer product)

void StmtMatmul::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

StmtMatmul::StmtMatmul(AstVariable* lhs,
                       AstMatmulMM* rhs)
    : StmtMatmulBase(lhs, rhs),
      _rhsMatmulMM(rhs),
      _rhsMatmulMV(NULL),
      _rhsMatmulVM(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

StmtMatmul::StmtMatmul(AstVariable* lhs,
                       AstMatmulMV* rhs)
    : StmtMatmulBase(lhs, rhs),
      _rhsMatmulMM(NULL),
      _rhsMatmulMV(rhs),
      _rhsMatmulVM(NULL)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

StmtMatmul::StmtMatmul(AstVariable* lhs,
                       AstMatmulVM* rhs)
    : StmtMatmulBase(lhs, rhs),
      _rhsMatmulMM(NULL),
      _rhsMatmulMV(NULL),
      _rhsMatmulVM(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // other statements move around GEMM
    buoyancyNeutral();
    descendAst(*lhs);
}

void StmtMatmul::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstMatmulMM* StmtMatmul::matmulPtr(void) const
{
    return _rhsMatmulMM;
}

AstMatmulMV* StmtMatmul::matvecPtr(void) const
{
    return _rhsMatmulMV;
}

AstMatmulVM* StmtMatmul::vecmatPtr(void) const
{
    return _rhsMatmulVM;
}

////////////////////////////////////////
// VisitAst

void StmtMatmul::visit(AstAccum& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstArrayMem& v)
{
}

void StmtMatmul::visit(AstCond& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstConvert& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstDotprod& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstExtension& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstFun1& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstFun2& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstFun3& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstGather& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstIdxdata& v)
{
}

void StmtMatmul::visit(AstLitdata& v)
{
}

void StmtMatmul::visit(AstMakedata& v)
{
}

void StmtMatmul::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstOpenCL& v)
{
}

void StmtMatmul::visit(AstReadout& v)
{
}

void StmtMatmul::visit(AstRNGnormal& v)
{
}

void StmtMatmul::visit(AstRNGuniform& v)
{
}

void StmtMatmul::visit(AstScalar& v)
{
}

void StmtMatmul::visit(AstTranspose& v)
{
    descendAst(v);
}

void StmtMatmul::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
