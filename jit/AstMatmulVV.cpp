// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulVV.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// outer product

AstMatmulVV::AstMatmulVV(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(bargRight->W(),
                    bargLeft->W(),
                    bargLeft->isDouble() || bargRight->isDouble(),
                    bargLeft,
                    bargRight)
{
}

void AstMatmulVV::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
