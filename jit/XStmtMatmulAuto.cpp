// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtMatmulAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned matrix multiply kernel
// request to load cache
// (all cases except outer product)

XStmtMatmulAuto::XStmtMatmulAuto(AstVariable* lhs,
                                 const AstMatmulMM* rhs)
    : XStmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

XStmtMatmulAuto::XStmtMatmulAuto(AstVariable* lhs,
                                 const AstMatmulMV* rhs)
    : XStmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

XStmtMatmulAuto::XStmtMatmulAuto(AstVariable* lhs,
                                 const AstMatmulVM* rhs)
    : XStmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

void XStmtMatmulAuto::accept(VisitXStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
