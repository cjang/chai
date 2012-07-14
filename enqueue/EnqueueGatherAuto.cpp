// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Enqueue.hpp"
#include "OCLhacks.hpp"
#include "StmtGatherAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtGatherAuto& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    if (OCLhacks::singleton().supportImages( _memMgr.deviceNum() ))
    {
        _jitMemo.eligibleGather(s.dataPtr(), *_xid);
    }
}

}; // namespace chai_internal
