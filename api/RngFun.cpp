// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// random number generation operation

// RNG function object
RngFun::RngFun(RNGBase32& generator,
               const uint32_t opCode,
               const size_t W,
               const size_t H,
               const size_t slots)
    : _W(W),
      _H(H),
      _slots(slots)
{
    _rngBC.push(static_cast<double>(slots));
    _rngBC.push(static_cast<double>(H));
    _rngBC.push(static_cast<double>(W));
    _rngBC.push(static_cast<double>(generator.rngSeed()));
    _rngBC.push(static_cast<double>(generator.rngVariant()));
    _rngBC.push(opCode);
    generator.incSeed(W * H * slots);
}

RngFun::RngFun(RNGBase64& generator,
               const uint32_t opCode,
               const size_t W,
               const size_t H,
               const size_t slots)
    : _W(W),
      _H(H),
      _slots(slots)
{
    _rngBC.push(static_cast<double>(slots));
    _rngBC.push(static_cast<double>(H));
    _rngBC.push(static_cast<double>(W));
    _rngBC.push(static_cast<double>(generator.rngSeed()));
    _rngBC.push(static_cast<double>(generator.rngVariant()));
    _rngBC.push(opCode);
    generator.incSeed(W * H * slots);
}

RngFun::RngFun(RNGf32& generator,
               const size_t W,
               const size_t H,
               const size_t slots,
               const float minLimit,
               const float maxLimit)
    : _W(W),
      _H(H),
      _slots(slots)
{
    _rngBC.push(static_cast<double>(maxLimit));
    _rngBC.push(static_cast<double>(minLimit));
    _rngBC.push(static_cast<double>(slots));
    _rngBC.push(static_cast<double>(H));
    _rngBC.push(static_cast<double>(W));
    _rngBC.push(static_cast<double>(generator.rngSeed()));
    _rngBC.push(static_cast<double>(generator.rngVariant()));
    _rngBC.push(ByteCodes::rng_uniform_make_f32);
    generator.incSeed(W * H * slots);
}

RngFun::RngFun(RNGf64& generator,
               const size_t W,
               const size_t H,
               const size_t slots,
               const double minLimit,
               const double maxLimit)
    : _W(W),
      _H(H),
      _slots(slots)
{
    _rngBC.push(static_cast<double>(maxLimit));
    _rngBC.push(static_cast<double>(minLimit));
    _rngBC.push(static_cast<double>(slots));
    _rngBC.push(static_cast<double>(H));
    _rngBC.push(static_cast<double>(W));
    _rngBC.push(static_cast<double>(generator.rngSeed()));
    _rngBC.push(static_cast<double>(generator.rngVariant()));
    _rngBC.push(ByteCodes::rng_uniform_make_f64);
    generator.incSeed(W * H * slots);
}

void RngFun::accept(Stak<BC>& v) const
{
    v.push(_rngBC);
}

void RngFun::accept(stack< ArrayDim >& v) const
{
    v.push( ArrayDim(_W, _H, _slots) );
}

}; // namespace chai
