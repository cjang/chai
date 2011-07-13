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
#include "XStmtReduce.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// reduction to scalar

void XStmtReduce::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

XStmtReduce::XStmtReduce(AstVariable* lhs,
                         AstAccum* rhs,
                         const bool convert)
    : _rhsAccum(rhs),
      _rhsDotprod(NULL),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
    descendAst(*lhs);
}

XStmtReduce::XStmtReduce(AstVariable* lhs,
                         AstDotprod* rhs,
                         const bool convert)
    : _rhsAccum(NULL),
      _rhsDotprod(rhs),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
    descendAst(*lhs);
}

bool XStmtReduce::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtReduce::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstAccum* XStmtReduce::accumPtr(void) const
{
    return _rhsAccum;
}

AstDotprod* XStmtReduce::dotprodPtr(void) const
{
    return _rhsDotprod;
}

bool XStmtReduce::isConvert(void) const
{
    return _isConvert;
}

////////////////////////////////////////
// VisitAst

void XStmtReduce::visit(AstAccum& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstArrayMem& v)
{
}

void XStmtReduce::visit(AstBinop& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstCond& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstConvert& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstDotprod& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstGather& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstIdxdata& v)
{
}

void XStmtReduce::visit(AstIsomorph& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstLitdata& v)
{
}

void XStmtReduce::visit(AstMakedata& v)
{
}
void XStmtReduce::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void XStmtReduce::visit(AstReadout& v)
{
}

void XStmtReduce::visit(AstRNGnormal& v)
{
}

void XStmtReduce::visit(AstRNGuniform& v)
{
}

void XStmtReduce::visit(AstScalar& v)
{
}

void XStmtReduce::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
