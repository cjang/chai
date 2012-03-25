// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun1.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// single argument functions

AstFun1::AstFun1(const SimpleFun1& fun,
                 BaseAst* barg)
    : BaseAst(barg->W(), barg->H(), barg->precision()),
      _fun(fun)
{
    pushArg(barg);
}

const SimpleFun1& AstFun1::fun(void) const
{
    return _fun;
}

void AstFun1::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
