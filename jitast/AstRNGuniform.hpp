// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_RNG_UNIFORM_HPP_
#define _CHAI_AST_RNG_UNIFORM_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_u32
// rng_uniform_make_i32
// rng_uniform_make_f32
// rng_uniform_make_f64

class AstRNGuniform : public BaseAst
{
    const int      _rngVariant;
    const uint64_t _rngSeed;
    const double   _minLimit;
    const double   _maxLimit;
    const size_t   _prec;

    const bool     _hasLimits;

public:
    AstRNGuniform(const int variant,
                  const uint64_t seed,
                  const size_t PREC,
                  const size_t W,
                  const size_t H,
                  const size_t slots);

    AstRNGuniform(const int variant,
                  const uint64_t seed,
                  const size_t PREC,
                  const size_t W,
                  const size_t H,
                  const size_t slots,
                  const double minLimit,
                  const double maxLimit);

    int rngVariant(void) const;
    uint64_t rngSeed(void) const;

    bool hasLimits(void) const;
    double minLimit(void) const;
    double maxLimit(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
