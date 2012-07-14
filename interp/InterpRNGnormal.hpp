// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_RNG_NORMAL_HPP_
#define _CHAI_INTERP_RNG_NORMAL_HPP_

#include "BaseInterp.hpp"
#include "InterpRNG.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_f32
// rng_normal_make_f64

class InterpRNGnormal : public BaseInterp
{
    const size_t _prec;
    InterpRNG&   _gen;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpRNGnormal(const size_t PREC, InterpRNG& gen);
};

}; // namespace chai_internal

#endif
