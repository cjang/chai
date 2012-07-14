// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstOpenCL.hpp"
#include "AstVariable.hpp"
#include "StmtOpenCL.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// inline OpenCL

StmtOpenCL::StmtOpenCL(AstOpenCL* rhs)
    : _rhsOpenCL(rhs)
{
    // inline OpenCL code does not rise or sink
    buoyancyNeutral();
}

bool StmtOpenCL::randomness(void) const
{
    return false;
}

bool StmtOpenCL::swappable(const Stmt& other) const
{
    return false;
}

void StmtOpenCL::accept(VisitStmt& v)
{
    v.visit(*this);
}

AstOpenCL* StmtOpenCL::openclPtr(void) const
{
    return _rhsOpenCL;
}

}; // namespace chai_internal
