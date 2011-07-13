// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMakedata.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// make1_f32, make1_f64, make2_f32, make2_f64

AstMakedata::AstMakedata(const vector< FrontMem* >& frontMem,
                         BackMem* backMem,
                         const size_t W,
                         const size_t H,
                         const bool isDouble)
    : BaseAst(W, H, isDouble),
      _frontMem(frontMem),
      _backMem(backMem) { }

const vector< FrontMem* >& AstMakedata::frontMem(void) const
{
    return _frontMem;
}

BackMem* AstMakedata::backMem(void) const
{
    return _backMem;
}

void AstMakedata::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
