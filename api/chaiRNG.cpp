// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// random number generators

#define DEFN_RNG(FP, X) \
\
RNG ## FP :: RNG ## FP (const RNGVariant variant, const X seed) \
    : _rngVariant(variant), \
      _rngSeed(seed) { } \
\
RNGVariant RNG ## FP ::rngVariant(void) const { return _rngVariant; } \
\
X RNG ## FP ::rngSeed(void) const { return _rngSeed; } \
\
void RNG ## FP ::incSeed(void) { _rngSeed++; }

DEFN_RNG(u32, uint32_t)
DEFN_RNG(i32, uint32_t)
DEFN_RNG(f32, uint32_t)
DEFN_RNG(f64, uint64_t)

#define DEFN_RNGFUN(FP, X) \
\
RngFun::RngFun(RNG ## FP & generator, \
               const size_t length, \
               const size_t step, \
               const X minLimit, \
               const X maxLimit) \
    : _length(length) { \
    _rngBC.push(static_cast<double>(maxLimit)); \
    _rngBC.push(static_cast<double>(minLimit)); \
    _rngBC.push(static_cast<double>(step)); \
    _rngBC.push(static_cast<double>(length)); \
    _rngBC.push(static_cast<double>(generator.rngSeed())); \
    _rngBC.push(static_cast<double>(generator.rngVariant())); \
    _rngBC.push(ByteCodes::rng_uniform_make_ ## FP); \
    generator.incSeed(); } \
\
RngFun::RngFun(RNG ## FP & generator, \
               const size_t length) \
    : _length(length) { \
    _rngBC.push(static_cast<double>(length)); \
    _rngBC.push(static_cast<double>(generator.rngSeed())); \
    _rngBC.push(static_cast<double>(generator.rngVariant())); \
    _rngBC.push(ByteCodes::rng_normal_make_ ## FP); \
    generator.incSeed(); }

DEFN_RNGFUN(u32, uint32_t)
DEFN_RNGFUN(i32, int32_t)
DEFN_RNGFUN(f32, float)
DEFN_RNGFUN(f64, double)

void RngFun::accept(Stak<BC>& v) const
{
    v.push(_rngBC);
}

void RngFun::accept(stack< ArrayDim >& v) const
{
    v.push( ArrayDim(_length, 1, 1) );
}

RngFun rng_uniform_make(RNGu32& generator,
                        const size_t length,
                        const size_t step)
{
    return RngFun(generator, length, step, 0, 0);
}

RngFun rng_uniform_make(RNGi32& generator,
                        const size_t length,
                        const size_t step)
{
    return RngFun(generator, length, step, 0, 0);
}

RngFun rng_uniform_make(RNGf32& generator,
                        const size_t length,
                        const size_t step,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, length, step, minLimit, maxLimit);
}

RngFun rng_uniform_make(RNGf64& generator,
                        const size_t length,
                        const size_t step,
                        const float minLimit,
                        const float maxLimit)
{
    return RngFun(generator, length, step, minLimit, maxLimit);
}

RngFun rng_normal_make(RNGf32& generator,
                       const size_t length)
{
    return RngFun(generator, length);
}

RngFun rng_normal_make(RNGf64& generator,
                       const size_t length)
{
    return RngFun(generator, length);
}

}; // namespace chai
