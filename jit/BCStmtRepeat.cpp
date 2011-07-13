// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BCStmtRepeat.hpp"
#include "PrintBC.hpp"

using namespace std;

namespace chai_internal {

BCStmtRepeat::BCStmtRepeat(const size_t reps,
                           BCStmtCompound* s)
    : _reps(reps),
      _stmt(s) { }

BCStmtRepeat::~BCStmtRepeat(void)
{
    delete _stmt;
}

uint64_t BCStmtRepeat::hashCode(void)
{
    return _stmt->hashCode();
}

void BCStmtRepeat::debug(ostream& os)
{
    os << "BEGIN Repeat(" << hashCode() << ") " << _reps << endl;
    PrintBC v(os, " ");
    _stmt->debug(os);
    os << "END Repeat(" << hashCode() << ") " << _reps << endl;
}

size_t BCStmtRepeat::numReps(void) const
{
    return _reps;
}

BCStmtCompound* BCStmtRepeat::stuffInside(void) const
{
    return _stmt;
}

void BCStmtRepeat::accept(VisitBCStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
