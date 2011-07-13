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
#include "XStmtGather.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// reduction to scalar

void XStmtGather::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

XStmtGather::XStmtGather(AstVariable* lhs,
                         AstGather* rhs,
                         const bool convert)
    : _rhsGather(rhs),
      _isConvert(convert)
{
    // set LHS variable
    lhsVariable(lhs);

    // code sinks
    buoyancySink();
    descendAst(*lhs);
}

bool XStmtGather::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtGather::accept(VisitXStmt& v)
{
    v.visit(*this);
}

AstGather* XStmtGather::gatherPtr(void) const
{
    return _rhsGather;
}

bool XStmtGather::isConvert(void) const
{
    return _isConvert;
}

////////////////////////////////////////
// VisitAst

void XStmtGather::visit(AstAccum& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstArrayMem& v)
{
}

void XStmtGather::visit(AstBinop& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstCond& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstConvert& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstDotprod& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstGather& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstIdxdata& v)
{
}

void XStmtGather::visit(AstIsomorph& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstLitdata& v)
{
}

void XStmtGather::visit(AstMakedata& v)
{
}
void XStmtGather::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void XStmtGather::visit(AstReadout& v)
{
}

void XStmtGather::visit(AstRNGnormal& v)
{
}

void XStmtGather::visit(AstRNGuniform& v)
{
}

void XStmtGather::visit(AstScalar& v)
{
}

void XStmtGather::visit(AstVariable& v)
{
    rhsVariable(&v);
}

}; // namespace chai_internal
