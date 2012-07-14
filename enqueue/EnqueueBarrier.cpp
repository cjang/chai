// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Enqueue.hpp"
#include "StmtBarrier.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtBarrier& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    _func->barrier();
}

}; // namespace chai_internal
