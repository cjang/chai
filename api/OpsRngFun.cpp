// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// random number generator operation

// RNG functions
RngFun rng_uniform_make(RNGu32& generator, const size_t W)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_u32, W, 1, 1);
}

RngFun rng_uniform_make(RNGu32& generator, const size_t W, const size_t H)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_u32, W, H, 1);
}

RngFun rng_uniform_make(RNGu32& generator,
                        const size_t W, const size_t H, const size_t slots)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_u32, W, H, slots);
}

RngFun rng_uniform_make(RNGi32& generator, const size_t W)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_i32, W, 1, 1);
}

RngFun rng_uniform_make(RNGi32& generator, const size_t W, const size_t H)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_i32, W, H, 1);
}

RngFun rng_uniform_make(RNGi32& generator,
                        const size_t W, const size_t H, const size_t slots)
{
    return RngFun(generator, ByteCodes::rng_uniform_make_i32, W, H, slots);
}

RngFun rng_uniform_make(RNGf32& generator, const size_t W,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, 1, 1, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf32& generator, const size_t W, const size_t H,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, H, 1, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf32& generator,
                        const size_t W, const size_t H, const size_t slots,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, H, slots, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf64& generator, const size_t W,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, 1, 1, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf64& generator, const size_t W, const size_t H,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, H, 1, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf64& generator,
                        const size_t W, const size_t H, const size_t slots,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, W, H, slots, minLimit, maxLimit);
}

RngFun rng_normal_make(RNGf32& generator, const size_t W)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f32, W, 1, 1);
}

RngFun rng_normal_make(RNGf32& generator, const size_t W, const size_t H)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f32, W, H, 1);
}

RngFun rng_normal_make(RNGf32& generator,
                       const size_t W, const size_t H, const size_t slots)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f32, W, H, slots);
}

RngFun rng_normal_make(RNGf64& generator, const size_t W)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f64, W, 1, 1);
}

RngFun rng_normal_make(RNGf64& generator, const size_t W, const size_t H)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f64, W, H, 1);
}

RngFun rng_normal_make(RNGf64& generator,
                       const size_t W, const size_t H, const size_t slots)
{
    return RngFun(generator, ByteCodes::rng_normal_make_f64, W, H, slots);
}

}; // namespace chai
