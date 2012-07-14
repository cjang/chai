// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstTranspose.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// transpose

AstTranspose::AstTranspose(BaseAst* barg)
    : BaseAst(barg->prec(),
              barg->H(),
              barg->W(),
              barg->slots(),
              barg->randomness())
{
    pushArg(barg);
}

void AstTranspose::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
