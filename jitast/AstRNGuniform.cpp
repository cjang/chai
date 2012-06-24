// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_uniform_make_u32, rng_uniform_make_i32
// rng_uniform_make_f32, rng_uniform_make_f64

AstRNGuniform::AstRNGuniform(const int variant,
                             const uint64_t seed,
                             const size_t len,
                             const size_t step,
                             const double minlimit,
                             const double maxlimit,
                             const size_t precision)
    : BaseAst(len, 1, precision),
      _rngVariant(variant),
      _rngSeed(seed),
      _step(step),
      _minlimit(minlimit),
      _maxlimit(maxlimit),
      _precision(precision) { }

int AstRNGuniform::rngVariant(void) const
{
    return _rngVariant;
}

uint64_t AstRNGuniform::rngSeed(void) const
{
    return _rngSeed;
}

size_t AstRNGuniform::step(void) const
{
    return _step;
}

double AstRNGuniform::minlimit(void) const
{
    return _minlimit;
}

double AstRNGuniform::maxlimit(void) const
{
    return _maxlimit;
}

void AstRNGuniform::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
