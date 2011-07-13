// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstAccum::AstAccum(BaseAst* barg,
                   const bool takeAvg)
    : BaseAst(1, 1, barg->isDouble()),
      _takeAvg(takeAvg),
      _argW(barg->W()),
      _argH(barg->H())
{
    pushArg(barg);
}

bool AstAccum::takeAvg(void) const
{
    return _takeAvg;
}

size_t AstAccum::argW(void) const
{
    return _argW;
}

size_t AstAccum::argH(void) const
{
    return _argH;
}

void AstAccum::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
