// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun2.hpp"
#include "TransFun2.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransFun2::sub_eval(void) const
{
    return
        new AstFun2(_fun,
                    _infix,
                    _argStack[0],
                    _argStack[1]);
}

TransFun2::TransFun2(const string& fun,
                     const bool infix)
    : BaseTrans(0, 2),
      _fun(fun),
      _infix(infix) { }

}; // namespace chai_internal
