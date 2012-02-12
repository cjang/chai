// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMM.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// matrix multiply

AstMatmulMM::AstMatmulMM(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(bargRight->W(),
                    bargLeft->H(),
                    bargLeft->isDouble() || bargRight->isDouble(),
                    bargLeft,
                    bargRight)
{
}

void AstMatmulMM::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
