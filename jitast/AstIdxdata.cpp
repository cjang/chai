// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstIdxdata.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// index1_u32, index1_i32, index2_u32, index2_i32
// index1_f32, index1_f64, index2_f32, index2_f64

AstIdxdata::AstIdxdata(const size_t index,
                       const size_t W,
                       const size_t precision)
    : BaseAst(W, 1, precision),
      _index(index) { }

AstIdxdata::AstIdxdata(const size_t index,
                       const size_t W,
                       const size_t H,
                       const size_t precision)
    : BaseAst(W, H, precision),
      _index(index) { }

size_t AstIdxdata::index(void) const
{
    return _index;
}

void AstIdxdata::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
