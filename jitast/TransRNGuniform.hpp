// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_RNG_UNIFORM_HPP_
#define _CHAI_TRANS_RNG_UNIFORM_HPP_

#include <cstddef>

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransRNGuniform : public BaseTrans
{
    const size_t _prec;
    const bool   _hasLimits;

    static size_t numberArgs(const size_t PREC);

protected:
    BaseAst* sub_eval(void) const;

public:
    // rng_uniform_make_u32
    // rng_uniform_make_i32
    // rng_uniform_make_f32
    // rng_uniform_make_f64
    TransRNGuniform(const size_t PREC);
};

}; // namespace chai_internal

#endif
