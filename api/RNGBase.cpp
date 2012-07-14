// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// random number generators

// 32 bit generator
RNGu32::RNGu32(const RNGVariant v, const uint32_t s) : RNGBase32(v, s) { }
RNGi32::RNGi32(const RNGVariant v, const uint32_t s) : RNGBase32(v, s) { }
RNGf32::RNGf32(const RNGVariant v, const uint32_t s) : RNGBase32(v, s) { }

RNGBase32::RNGBase32(const RNGVariant variant, const uint32_t seed)
    : _rngVariant(variant),
      _rngSeed(seed) { }

RNGVariant RNGBase32::rngVariant(void) const
{
    return _rngVariant;
}

uint32_t RNGBase32::rngSeed(void) const
{
    return _rngSeed;
}

void RNGBase32::incSeed(const uint32_t s)
{
    _rngSeed += s;
}

// 64 bit generator
RNGf64::RNGf64(const RNGVariant v, const uint64_t s) : RNGBase64(v, s) { }

RNGBase64::RNGBase64(const RNGVariant variant, const uint64_t seed)
    : _rngVariant(variant),
      _rngSeed(seed) { }

RNGVariant RNGBase64::rngVariant(void) const
{
    return _rngVariant;
}

uint64_t RNGBase64::rngSeed(void) const
{
    return _rngSeed;
}

void RNGBase64::incSeed(const uint64_t s)
{
    _rngSeed += s;
}

}; // namespace chai
