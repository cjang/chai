// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun2.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// functions with two arguments

AstFun2::AstFun2(const string& fun,
                 const bool infix,
                 BaseAst* bargLeft,
                 BaseAst* bargRight)
    : BaseAst(max<size_t>(bargLeft->prec(), bargRight->prec()),
              max<size_t>(bargLeft->W(), bargRight->W()),
              max<size_t>(bargLeft->H(), bargRight->H()),
              max<size_t>(bargLeft->slots(), bargRight->slots()),
              bargLeft->randomness() || bargRight->randomness()),
      _fun(fun),
      _infix(infix)
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

const string& AstFun2::fun(void) const
{
    return _fun;
}

bool AstFun2::infix(void) const
{
    return _infix;
}

void AstFun2::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
