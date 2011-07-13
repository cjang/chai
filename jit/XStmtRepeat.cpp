// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtRepeat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// repeat AST statement

XStmtRepeat::XStmtRepeat(const size_t reps)
    : _comStmt(NULL),
      _reps(reps),
      _loopRoll(true)
{
    // for-loops don't rise or sink
    buoyancyNeutral();
}

XStmtRepeat::~XStmtRepeat(void)
{
    delete _comStmt;
}

void XStmtRepeat::setCompound(XStmtCompound* comStmt)
{
    _comStmt = comStmt;
}

bool XStmtRepeat::getLoopRoll(void) const
{
    return _loopRoll;
}

void XStmtRepeat::setLoopRoll(const bool loopRoll)
{
    _loopRoll = loopRoll;
}

bool XStmtRepeat::swappable(const XStmt& other) const
{
    // repetition does not affect swappability
    return _comStmt->swappable(other);
}

void XStmtRepeat::accept(VisitXStmt& v)
{
    v.visit(*this);
}

size_t XStmtRepeat::numReps(void) const
{
    return _reps;
}

XStmtCompound* XStmtRepeat::stuffInside(void) const
{
    return _comStmt;
}

}; // namespace chai_internal
