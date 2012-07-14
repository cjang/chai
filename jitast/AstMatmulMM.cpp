// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMM.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// matrix multiply

AstMatmulMM::AstMatmulMM(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(max<size_t>(bargLeft->prec(), bargRight->prec()),
                    bargRight->W(),
                    bargLeft->H(),
                    max<size_t>(bargLeft->slots(), bargRight->slots()),
                    false, // matrix * matrix is separate kernel
                    bargLeft,
                    bargRight) { }

void AstMatmulMM::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
