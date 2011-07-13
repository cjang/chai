// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGnormal.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_normal_make_f32, rng_normal_make_f64

AstRNGnormal::AstRNGnormal(const unsigned int seed,
                           const size_t variant,
                           const size_t len,
                           const bool isDouble)
    : BaseAst(len, 1, isDouble),
      _seed(seed),
      _variant(variant) { }

void AstRNGnormal::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
