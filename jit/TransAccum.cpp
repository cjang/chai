// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"
#include "TransAccum.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransAccum::sub_eval(void) const
{
    return
        new AstAccum(_argStack[0],
                     _takeAvg );
}

TransAccum::TransAccum(const bool takeAvg)
    : BaseTrans(0, 1),
      _takeAvg(takeAvg) { }

}; // namespace chai_internal
