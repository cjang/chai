// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun2.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// functions with two arguments

AstFun2::AstFun2(const SimpleFun2& fun,
                 BaseAst* bargLeft,
                 BaseAst* bargRight)
    : BaseAst(max<size_t>(bargLeft->W(), bargRight->W()),
              max<size_t>(bargLeft->H(), bargRight->H()),
              bargLeft->isDouble() || bargRight->isDouble()),
      _fun(fun),
      _leftW(bargLeft->W()),
      _leftH(bargLeft->H()),
      _rightW(bargRight->W()),
      _rightH(bargRight->H())
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

const SimpleFun2& AstFun2::fun(void) const
{
    return _fun;
}

size_t AstFun2::leftW(void) const
{
    return _leftW;
}

size_t AstFun2::leftH(void) const
{
    return _leftH;
}

size_t AstFun2::rightW(void) const
{
    return _rightW;
}

size_t AstFun2::rightH(void) const
{
    return _rightH;
}

void AstFun2::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
