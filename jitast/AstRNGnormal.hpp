// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_RNG_NORMAL_HPP_
#define _CHAI_AST_RNG_NORMAL_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_u32, rng_normal_make_i32
// rng_normal_make_f32, rng_normal_make_f64

class AstRNGnormal : public BaseAst
{
    const int      _rngVariant;
    const uint64_t _rngSeed;

public:
    AstRNGnormal(const int variant,
                 const uint64_t seed,
                 const size_t len,
                 const size_t precision);

    int rngVariant(void) const;
    uint64_t rngSeed(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
