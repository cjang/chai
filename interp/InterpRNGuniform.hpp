// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_RNG_UNIFORM_HPP_
#define _CHAI_INTERP_RNG_UNIFORM_HPP_

#include "BaseInterp.hpp"
#include "InterpRNG.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_f32, rng_uniform_make_f64

class InterpRNGuniform : public BaseInterp
{
    const bool _isDP;
    InterpRNG& _gen;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpRNGuniform(const bool isDP, InterpRNG& gen);
};

}; // namespace chai_internal

#endif
