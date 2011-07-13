// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstBinop.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstBinop::AstBinop(const SimpleFun2& fun,
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

const SimpleFun2& AstBinop::fun(void) const
{
    return _fun;
}

size_t AstBinop::leftW(void) const
{
    return _leftW;
}

size_t AstBinop::leftH(void) const
{
    return _leftH;
}

size_t AstBinop::rightW(void) const
{
    return _rightW;
}

size_t AstBinop::rightH(void) const
{
    return _rightH;
}

void AstBinop::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
