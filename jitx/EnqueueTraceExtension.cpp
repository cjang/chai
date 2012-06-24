// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "EnqueueTrace.hpp"
#include "StmtExtension.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtExtension& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    if (! s.extensionEnqueue(_memMgr, _vt))
        _failure = true;
}

}; // namespace chai_internal
