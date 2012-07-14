// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun1.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// single argument functions

AstFun1::AstFun1(const string& fun,
                 BaseAst* barg)
    : BaseAst(barg->prec(),
              barg->W(),
              barg->H(),
              barg->slots(),
              barg->randomness()),
      _fun(fun)
{
    pushArg(barg);
}

const string& AstFun1::fun(void) const
{
    return _fun;
}

void AstFun1::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
