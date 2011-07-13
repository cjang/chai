// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstBinop.hpp"
#include "TransBinop.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransBinop::sub_eval(void) const
{
    return
        new AstBinop(_fun,
                     _argStack[0],
                     _argStack[1]);
}

TransBinop::TransBinop(const SimpleFun2& fun)
    : BaseTrans(0, 2),
      _fun(fun) { }

}; // namespace chai_internal
