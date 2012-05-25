// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtGatherAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// auto-tuned gathering uses images when
// possible

StmtGatherAuto::StmtGatherAuto(AstVariable* dataVariable)
    : _dataVariable(dataVariable)
{
    buoyancySurface();
}

bool StmtGatherAuto::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtGatherAuto::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstVariable* StmtGatherAuto::dataPtr(void) const
{
    return _dataVariable;
}

}; // namespace chai_internal
