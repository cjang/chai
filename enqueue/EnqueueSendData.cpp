// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BackMem.hpp"
#include "Enqueue.hpp"
#include "StmtSendData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtSendData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    BackMem* backMem = NULL;

    if (s.arrayMemPtr())
    {
        // array memory comes from intermediate variable values
        backMem = s.arrayMemPtr()->backMem();
    }
    else if (s.makedataPtr())
    {
        // make data is new array data
        backMem = s.makedataPtr()->backMem();
    }

    if (! _jitMemo.createArraySendData(s.lhsVariable(), *_xid, backMem))
        _failure = true;
}

}; // namespace chai_internal
