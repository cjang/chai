// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstDotprod.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// dot product

AstDotprod::AstDotprod(BaseAst* bargLeft,
                       BaseAst* bargRight)
    : BaseAst(max<size_t>(bargLeft->prec(), bargRight->prec()),
              1,
              1,
              max<size_t>(bargLeft->slots(), bargRight->slots()),
              bargLeft->randomness() || bargRight->randomness()),
      _insideW(max<size_t>(bargLeft->W(), bargRight->W())),
      _insideH(max<size_t>(bargLeft->H(), bargRight->H()))
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

size_t AstDotprod::insideW(void) const
{
    return _insideW;
}

size_t AstDotprod::insideH(void) const
{
    return _insideH;
}

void AstDotprod::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
