// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_RNG_NORMAL_HPP_
#define _CHAI_AST_RNG_NORMAL_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_u32, rng_normal_make_i32
// rng_normal_make_f32, rng_normal_make_f64

class AstRNGnormal : public BaseAst
{
    const unsigned int _seed;
    const size_t       _variant;

public:
    AstRNGnormal(const unsigned int seed,
                 const size_t variant,
                 const size_t len,
                 const size_t precision);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
