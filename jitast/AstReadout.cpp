// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstReadout.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// read_scalar, read1, read2

AstReadout::AstReadout(const vector< FrontMem* >& frontMem,
                       BackMem* backMem,
                       BaseAst* barg,
                       const size_t readVarDim)
    : BaseAst(barg->W(), barg->H(), barg->precision()),
      _frontMem(frontMem),
      _backMem(backMem),
      _readVarDim(readVarDim)
{
    pushArg(barg);
}

size_t AstReadout::readVarDim(void) const
{
    return _readVarDim;
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
