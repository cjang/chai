// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtRepeat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// repeat AST statement

StmtRepeat::StmtRepeat(const size_t reps)
    : _comStmt(NULL),
      _reps(reps),
      _loopRoll(true)
{
    // for-loops don't rise or sink
    buoyancyNeutral();
}

StmtRepeat::~StmtRepeat(void)
{
    delete _comStmt;
}

void StmtRepeat::setCompound(StmtCompound* comStmt)
{
    _comStmt = comStmt;
}

bool StmtRepeat::getLoopRoll(void) const
{
    return _loopRoll;
}

void StmtRepeat::setLoopRoll(const bool loopRoll)
{
    _loopRoll = loopRoll;
}

bool StmtRepeat::swappable(const Stmt& other) const
{
    // repetition does not affect swappability
    return _comStmt->swappable(other);
}

void StmtRepeat::accept(VisitStmt& v)
{
    v.visit(*this);
}

size_t StmtRepeat::numReps(void) const
{
    return _reps;
}

StmtCompound* StmtRepeat::stuffInside(void) const
{
    return _comStmt;
}

}; // namespace chai_internal
