// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "EnqueueTrace.hpp"
#include "OCLhacks.hpp"
#include "StmtGatherAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtGatherAuto& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    const size_t deviceIdx = _memMgr.getDeviceIndex();

    if (OCLhacks::singleton().supportImages(deviceIdx))
    {
        _jitMemo.eligibleGather(s.dataPtr(), *_xid);
    }
}

}; // namespace chai_internal
