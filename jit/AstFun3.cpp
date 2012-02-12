// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstFun3.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// functions with three arguments

AstFun3::AstFun3(const SimpleFun3& fun,
                 BaseAst* bargLeft,
                 BaseAst* bargMiddle,
                 BaseAst* bargRight)
    : BaseAst(max<size_t>(bargLeft->W(), bargMiddle->W(), bargRight->W()),
              max<size_t>(bargLeft->H(), bargMiddle->H(), bargRight->H()),
              bargLeft->isDouble() ||
              bargMiddle->isDouble() ||
              bargRight->isDouble()),
      _fun(fun),
      _leftW(bargLeft->W()),
      _leftH(bargLeft->H()),
      _middleW(bargMiddle->W()),
      _middleH(bargMiddle->H()),
      _rightW(bargRight->W()),
      _rightH(bargRight->H())
{
    pushArg(bargLeft);
    pushArg(bargMiddle);
    pushArg(bargRight);
}

const SimpleFun3& AstFun3::fun(void) const
{
    return _fun;
}

size_t AstFun3::leftW(void) const
{
    return _leftW;
}

size_t AstFun3::leftH(void) const
{
    return _leftH;
}

size_t AstFun3::middleW(void) const
{
    return _middleW;
}

size_t AstFun3::middleH(void) const
{
    return _middleH;
}

size_t AstFun3::rightW(void) const
{
    return _rightW;
}

size_t AstFun3::rightH(void) const
{
    return _rightH;
}

void AstFun3::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
