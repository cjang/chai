// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstCond.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// cond

AstCond::AstCond(BaseAst* bargPredicate,
                 BaseAst* bargTrue,
                 BaseAst* bargFalse)
    : BaseAst(max<size_t>(bargTrue->W(), bargFalse->W()),
              max<size_t>(bargTrue->H(), bargFalse->H()),
              bargTrue->isDouble() || bargFalse->isDouble()),
      _predicateW(bargPredicate->W()),
      _predicateH(bargPredicate->H()),
      _trueW(bargTrue->W()),
      _trueH(bargTrue->H()),
      _falseW(bargFalse->W()),
      _falseH(bargFalse->H())
{
    pushArg(bargPredicate);
    pushArg(bargTrue);
    pushArg(bargFalse);
}

size_t AstCond::predicateW(void) const
{
    return _predicateW;
}

size_t AstCond::predicateH(void) const
{
    return _predicateH;
}

size_t AstCond::trueW(void) const
{
    return _trueW;
}

size_t AstCond::trueH(void) const
{
    return _trueH;
}

size_t AstCond::falseW(void) const
{
    return _falseW;
}

size_t AstCond::falseH(void) const
{
    return _falseH;
}

void AstCond::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
