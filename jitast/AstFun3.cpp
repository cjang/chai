// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun3.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// functions with three arguments

AstFun3::AstFun3(const string& fun,
                 BaseAst* bargLeft,
                 BaseAst* bargMiddle,
                 BaseAst* bargRight)
    : BaseAst(max<size_t>(bargLeft->prec(),
                          bargMiddle->prec(),
                          bargRight->prec()),
              max<size_t>(bargLeft->W(), bargMiddle->W(), bargRight->W()),
              max<size_t>(bargLeft->H(), bargMiddle->H(), bargRight->H()),
              max<size_t>(bargLeft->slots(),
                          bargMiddle->slots(),
                          bargRight->slots()),
              bargLeft->randomness() ||
              bargMiddle->randomness() ||
              bargRight->randomness()),
      _fun(fun)
{
    pushArg(bargLeft);
    pushArg(bargMiddle);
    pushArg(bargRight);
}

const string& AstFun3::fun(void) const
{
    return _fun;
}

void AstFun3::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
