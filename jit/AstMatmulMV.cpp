// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulMV.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// matrix vector multiply

AstMatmulMV::AstMatmulMV(BaseAst* bargLeft,
                         BaseAst* bargRight)
    : AstMatmulBase(bargLeft->H(),
                    1,
                    bargLeft->isDouble() || bargRight->isDouble(),
                    bargLeft,
                    bargRight)
{
}

void AstMatmulMV::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
