// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Enqueue.hpp"
#include "StmtRepeat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtRepeat& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // begin loop
    const size_t loopIdx = _func->loopBegin(0, s.numReps());
    _repeatIdx.push(loopIdx);

    // descend into body of loop
    s.stuffInside()->accept(*this);

    // end loop
    _func->loopEnd();
    _repeatIdx.pop();
}

}; // namespace chai_internal
