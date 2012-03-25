// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtMatmulAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned matrix multiply kernel
// request to load cache
// (all cases except outer product)

StmtMatmulAuto::StmtMatmulAuto(AstVariable* lhs,
                               const AstMatmulMM* rhs)
    : StmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

StmtMatmulAuto::StmtMatmulAuto(AstVariable* lhs,
                               const AstMatmulMV* rhs)
    : StmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

StmtMatmulAuto::StmtMatmulAuto(AstVariable* lhs,
                               const AstMatmulVM* rhs)
    : StmtMatmulBase(lhs, rhs)
{
    buoyancySurface();
}

void StmtMatmulAuto::accept(VisitStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
