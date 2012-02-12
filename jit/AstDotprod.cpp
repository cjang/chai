// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstDotprod.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// dot product

AstDotprod::AstDotprod(BaseAst* bargLeft,
                       BaseAst* bargRight)
    : BaseAst(1, 1, bargLeft->isDouble() || bargRight->isDouble()),
      _leftW(bargLeft->W()),
      _leftH(bargLeft->H()),
      _rightW(bargRight->W()),
      _rightH(bargRight->H())
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

size_t AstDotprod::leftW(void) const
{
    return _leftW;
}

size_t AstDotprod::leftH(void) const
{
    return _leftH;
}

size_t AstDotprod::rightW(void) const
{
    return _rightW;
}

size_t AstDotprod::rightH(void) const
{
    return _rightH;
}

void AstDotprod::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
