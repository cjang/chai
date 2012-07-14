// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstAccum::AstAccum(BaseAst* barg,
                   const bool takeAvg)
    : BaseAst(barg->prec(), 1, 1, barg->slots(), barg->randomness()),
      _takeAvg(takeAvg),
      _insideW(barg->W()),
      _insideH(barg->H())
{
    pushArg(barg);
}

bool AstAccum::takeAvg(void) const
{
    return _takeAvg;
}

size_t AstAccum::insideW(void) const
{
    return _insideW;
}

size_t AstAccum::insideH(void) const
{
    return _insideH;
}

void AstAccum::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
