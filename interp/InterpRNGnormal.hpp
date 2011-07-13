// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_RNG_NORMAL_HPP_
#define _CHAI_INTERP_RNG_NORMAL_HPP_

#include "BaseInterp.hpp"
#include "InterpRNG.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_f32, rng_normal_make_f64

class InterpRNGnormal : public BaseInterp
{
    const bool _isDP;
    InterpRNG& _gen;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpRNGnormal(const bool isDP, InterpRNG& gen);
};

}; // namespace chai_internal

#endif
