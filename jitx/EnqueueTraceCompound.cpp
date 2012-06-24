// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "EnqueueTrace.hpp"
#include "StmtCompound.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtCompound& s)
{
    for (vector< Stmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        if (_failure) return;

        (*it)->accept(*this);
    }
}

}; // namespace chai_internal
