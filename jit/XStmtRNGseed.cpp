// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtRNGseed.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// random number generation

XStmtRNGseed::XStmtRNGseed(const size_t len)
    : _length(len)
{
    buoyancyRise();
}

size_t XStmtRNGseed::length(void) const
{
    return _length;
}

bool XStmtRNGseed::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtRNGseed::accept(VisitXStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
