// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulVM.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstMatmulVM::AstMatmulVM(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : BaseAst(bargRight->W(),
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

size_t AstMatmulVM::leftW(void) const
{
    return _leftW;
}

size_t AstMatmulVM::leftH(void) const
{
    return _leftH;
}

size_t AstMatmulVM::rightW(void) const
{
    return _rightW;
}

size_t AstMatmulVM::rightH(void) const
{
    return _rightH;
}

void AstMatmulVM::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
