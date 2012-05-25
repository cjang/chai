// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_RNG_UNIFORM_HPP_
#define _CHAI_AST_RNG_UNIFORM_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_u32, rng_uniform_make_i32
// rng_uniform_make_f32, rng_uniform_make_f64

class AstRNGuniform : public BaseAst
{
    const int      _rngVariant;
    const uint64_t _rngSeed;
    const size_t   _step;
    const double   _minlimit;
    const double   _maxlimit;
    const size_t   _precision;

/*FIXME - remove this
    bool _insideRolledLoop;
*/

public:
    AstRNGuniform(const int variant,
                  const uint64_t seed,
                  const size_t len,
                  const size_t step,
                  const double minlimit,
                  const double maxlimit,
                  const size_t precision);

    int rngVariant(void) const;
    uint64_t rngSeed(void) const;

    size_t step(void) const;
    double minlimit(void) const;
    double maxlimit(void) const;

/*FIXME - remove this
    bool getInsideRolledLoop(void) const;
    void setInsideRolledLoop(void);
*/

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
