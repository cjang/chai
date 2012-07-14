// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGnormal.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_normal_make_f32
// rng_normal_make_f64

AstRNGnormal::AstRNGnormal(const int variant,
                           const uint64_t seed,
                           const size_t PREC,
                           const size_t W,
                           const size_t H,
                           const size_t slots)
    : BaseAst(PREC, W, H, slots, true),
      _rngVariant(variant),
      _rngSeed(seed) { }

int AstRNGnormal::rngVariant(void) const
{
    return _rngVariant;
}

uint64_t AstRNGnormal::rngSeed(void) const
{
    return _rngSeed;
}

void AstRNGnormal::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
