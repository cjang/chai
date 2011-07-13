// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstReadout.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// read_scalar, read1, read2

AstReadout::AstReadout(const vector< FrontMem* >& frontMem,
                       BackMem* backMem,
                       BaseAst* barg)
    : BaseAst(barg->W(), barg->H(), barg->isDouble()),
      _frontMem(frontMem),
      _backMem(backMem)
{
    pushArg(barg);
}

const vector< FrontMem* >& AstReadout::frontMem(void) const
{
    return _frontMem;
}

BackMem* AstReadout::backMem(void) const
{
    return _backMem;
}

void AstReadout::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
