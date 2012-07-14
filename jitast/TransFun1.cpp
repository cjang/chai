// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun1.hpp"
#include "TransFun1.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransFun1::sub_eval(void) const
{
    return
        new AstFun1(_fun,
                    _argStack[0]);
}

TransFun1::TransFun1(const string& fun)
    : BaseTrans(0, 1),
      _fun(fun) { }

}; // namespace chai_internal
