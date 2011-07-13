// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMV.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstMatmulMV::AstMatmulMV(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : BaseAst(bargLeft->H(),
              1,
              bargLeft->isDouble() || bargRight->isDouble()),
      _leftW(bargLeft->W()),
      _leftH(bargLeft->H()),
      _rightW(bargRight->W()),
      _rightH(bargRight->H())
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

size_t AstMatmulMV::leftW(void) const
{
    return _leftW;
}

size_t AstMatmulMV::leftH(void) const
{
    return _leftH;
}

size_t AstMatmulMV::rightW(void) const
{
    return _rightW;
}

size_t AstMatmulMV::rightH(void) const
{
    return _rightH;
}

void AstMatmulMV::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
