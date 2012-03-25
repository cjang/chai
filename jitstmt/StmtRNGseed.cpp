// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtRNGseed.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// random number generation

StmtRNGseed::StmtRNGseed(const size_t len)
    : _length(len)
{
    buoyancyRise();
}

size_t StmtRNGseed::length(void) const
{
    return _length;
}

bool StmtRNGseed::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtRNGseed::accept(VisitStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
