// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "AstRNGuniform.hpp"
#include "TransRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

size_t TransRNGuniform::numberArgs(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
            return 5;

        case (PrecType::Float) :
        case (PrecType::Double) :
            return 7;
    }
}

BaseAst* TransRNGuniform::sub_eval(void) const
{
    const int variant   = _argScalar[0];
    const uint64_t seed = _argScalar[1];
    const size_t W      = _argScalar[2];
    const size_t H      = _argScalar[3];
    const size_t slots  = _argScalar[4];

    if (_hasLimits)
    {
        const double minLimit = _argScalar[5];
        const double maxLimit = _argScalar[6];

        return
            new AstRNGuniform(variant,
                              seed,
                              _prec,
                              W,
                              H,
                              slots,
                              minLimit,
                              maxLimit);
    }
    else
    {
        return
            new AstRNGuniform(variant,
                              seed,
                              _prec,
                              W,
                              H,
                              slots);
    }
}

// rng_uniform_make_u32
// rng_uniform_make_i32
// rng_uniform_make_f32
// rng_uniform_make_f64
TransRNGuniform::TransRNGuniform(const size_t PREC)
    : BaseTrans(numberArgs(PREC), 0),
      _prec(PREC),
      _hasLimits(7 == numberArgs(PREC)) { }

}; // namespace chai_internal
