// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGnormal.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_normal_make_u32, rng_normal_make_i64
// rng_normal_make_f32, rng_normal_make_f64

AstRNGnormal::AstRNGnormal(const int variant,
                           const uint64_t seed,
                           const size_t len,
                           const size_t precision)
    : BaseAst(len, 1, precision),
      _rngVariant(variant),
      _rngSeed(seed)
/*FIXME - remove this,
      _insideRolledLoop(false)
*/
 { }

int AstRNGnormal::rngVariant(void) const
{
    return _rngVariant;
}

uint64_t AstRNGnormal::rngSeed(void) const
{
    return _rngSeed;
}

/*FIXME - remove this
bool AstRNGnormal::getInsideRolledLoop(void) const
{
    return _insideRolledLoop;
}

void AstRNGnormal::setInsideRolledLoop(void)
{
    _insideRolledLoop = true;
}
*/

void AstRNGnormal::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
