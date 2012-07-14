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
#include "StmtReduce.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// reduction to scalar

void StmtReduce::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

StmtReduce::StmtReduce(AstVariable* lhs,
                       AstAccum* rhs)
    : _rhsAccum(rhs),
      _rhsDotprod(NULL),
      _rhsW(rhs->insideW()),
      _rhsH(rhs->insideH()),
      _nestStmt()
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
    descendAst(*lhs);
}

StmtReduce::StmtReduce(AstVariable* lhs,
                       AstDotprod* rhs)
    : _rhsAccum(NULL),
      _rhsDotprod(rhs),
      _rhsW(rhs->insideW()),
      _rhsH(rhs->insideH()),
      _nestStmt()
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
    descendAst(*lhs);
}

bool StmtReduce::randomness(void) const
{
    return lhsVariable()->randomness();
}

bool StmtReduce::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

const vector< Stmt* >& StmtReduce::nestStmt(void) const
{
    return _nestStmt;
}

void StmtReduce::nestStmt(Stmt* s)
{
    _nestStmt.push_back(s);
}

void StmtReduce::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstAccum* StmtReduce::accumPtr(void) const
{
    return _rhsAccum;
}

AstDotprod* StmtReduce::dotprodPtr(void) const
{
    return _rhsDotprod;
}

size_t StmtReduce::rhsW(void) const
{
    return _rhsW;
}

size_t StmtReduce::rhsH(void) const
{
    return _rhsH;
}

////////////////////////////////////////
// VisitAst

void StmtReduce::visit(AstAccum& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstArrayMem& v)
{
}

void StmtReduce::visit(AstCond& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstConvert& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstDotprod& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstExtension& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstFun1& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstFun2& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstFun3& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstGather& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstIdxdata& v)
{
}

void StmtReduce::visit(AstLitdata& v)
{
}

void StmtReduce::visit(AstMakedata& v)
{
}

void StmtReduce::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstOpenCL& v)
{
}

void StmtReduce::visit(AstReadout& v)
{
}

void StmtReduce::visit(AstRNGnormal& v)
{
}

void StmtReduce::visit(AstRNGuniform& v)
{
}

void StmtReduce::visit(AstScalar& v)
{
}

void StmtReduce::visit(AstTranspose& v)
{
    descendAst(v);
}

void StmtReduce::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
