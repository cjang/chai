// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstArrayMem.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// array memory

AstArrayMem::AstArrayMem(const vector< FrontMem* >& frontMem,
                         BackMem* backMem,
                         const uint32_t variable,
                         const uint32_t version)
    : BaseAst(frontMem.front()->W(),
              frontMem.front()->H(),
              frontMem.front()->isDouble()),
      _frontMem(frontMem),
      _backMem(backMem),
      _variable(variable),
      _version(version) { }

const vector< FrontMem* >& AstArrayMem::frontMem(void) const
{
    return _frontMem;
}

BackMem* AstArrayMem::backMem(void) const
{
    return _backMem;
}

uint32_t AstArrayMem::variable(void) const
{
    return _variable;
}

uint32_t AstArrayMem::version(void) const
{
    return _version;
}

void AstArrayMem::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
