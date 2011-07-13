// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMM.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstMatmulMM::AstMatmulMM(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : BaseAst(bargRight->W(),
              bargLeft->H(),
              bargLeft->isDouble() || bargRight->isDouble()),
      _leftW(bargLeft->W()),
      _leftH(bargLeft->H()),
      _rightW(bargRight->W()),
      _rightH(bargRight->H())
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

size_t AstMatmulMM::leftW(void) const
{
    return _leftW;
}

size_t AstMatmulMM::leftH(void) const
{
    return _leftH;
}

size_t AstMatmulMM::rightW(void) const
{
    return _rightW;
}

size_t AstMatmulMM::rightH(void) const
{
    return _rightH;
}

void AstMatmulMM::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
