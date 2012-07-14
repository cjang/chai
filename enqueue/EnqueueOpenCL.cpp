// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstOpenCL.hpp"
#include "Enqueue.hpp"
#include "StmtOpenCL.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void Enqueue::visit(StmtOpenCL& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    s.openclPtr()->accept(*this);
}

}; // namespace chai_internal
