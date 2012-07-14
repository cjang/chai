// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_uniform_make_u32
// rng_uniform_make_i32
// rng_uniform_make_f32
// rng_uniform_make_f64

AstRNGuniform::AstRNGuniform(const int variant,
                             const uint64_t seed,
                             const size_t PREC,
                             const size_t W,
                             const size_t H,
                             const size_t slots)
    : BaseAst(PREC, W, H, slots, true),
      _rngVariant(variant),
      _rngSeed(seed),
      _minLimit(0),
      _maxLimit(0),
      _prec(PREC),
      _hasLimits(false) { }

AstRNGuniform::AstRNGuniform(const int variant,
                             const uint64_t seed,
                             const size_t PREC,
                             const size_t W,
                             const size_t H,
                             const size_t slots,
                             const double minLimit,
                             const double maxLimit)
    : BaseAst(PREC, W, H, slots, true),
      _rngVariant(variant),
      _rngSeed(seed),
      _minLimit(minLimit),
      _maxLimit(maxLimit),
      _prec(PREC),
      _hasLimits(true) { }

int AstRNGuniform::rngVariant(void) const
{
    return _rngVariant;
}

uint64_t AstRNGuniform::rngSeed(void) const
{
    return _rngSeed;
}

bool AstRNGuniform::hasLimits(void) const
{
    return _hasLimits;
}

double AstRNGuniform::minLimit(void) const
{
    return _minLimit;
}

double AstRNGuniform::maxLimit(void) const
{
    return _maxLimit;
}

void AstRNGuniform::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
