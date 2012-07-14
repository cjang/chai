// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulVM.hpp"
#include "UtilFuns.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// vector matrix multiply

AstMatmulVM::AstMatmulVM(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(max<size_t>(bargLeft->prec(), bargRight->prec()),
                    bargRight->W(),
                    1,
                    max<size_t>(bargLeft->slots(), bargRight->slots()),
                    false, // vector * matrix is separate kernel
                    bargLeft,
                    bargRight) { }

void AstMatmulVM::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
