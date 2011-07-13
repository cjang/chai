// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstIsomorph.hpp"
#include "TransIsomorph.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransIsomorph::sub_eval(void) const
{
    return
        new AstIsomorph(_fun,
                        _argStack[0]);
}

TransIsomorph::TransIsomorph(const SimpleFun1& fun)
    : BaseTrans(0, 1),
      _fun(fun) { }

}; // namespace chai_internal
