// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstIsomorph.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// mean, sum

AstIsomorph::AstIsomorph(const SimpleFun1& fun,
                         BaseAst* barg)
    : BaseAst(barg->W(), barg->H(), barg->isDouble()),
      _fun(fun)
{
    pushArg(barg);
}

const SimpleFun1& AstIsomorph::fun(void) const
{
    return _fun;
}

void AstIsomorph::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
