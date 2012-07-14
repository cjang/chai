// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMV.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// matrix vector multiply

AstMatmulMV::AstMatmulMV(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(max<size_t>(bargLeft->prec(), bargRight->prec()),
                    bargLeft->H(),
                    1,
                    max<size_t>(bargLeft->slots(), bargRight->slots()),
                    false, // matrix * vector is separate kernel
                    bargLeft,
                    bargRight) { }

void AstMatmulMV::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
