// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun3.hpp"
#include "TransFun3.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransFun3::sub_eval(void) const
{
    return
        new AstFun3(_fun,
                    _argStack[0],
                    _argStack[1],
                    _argStack[2]);
}

TransFun3::TransFun3(const string& fun)
    : BaseTrans(0, 3),
      _fun(fun) { }

}; // namespace chai_internal
