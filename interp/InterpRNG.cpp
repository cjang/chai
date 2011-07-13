// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "InterpRNG.hpp"

namespace chai_internal {

////////////////////////////////////////
// random number generation abstraction

void InterpRNG::seed(const unsigned int seed)
{
    memset(&_randbuf, 0, sizeof(struct random_data));

    // salt random seed with address of this object
    // every thread should have its own RNG object
    const unsigned int salted_seed =
        reinterpret_cast<unsigned long>(this) ^ seed;

    initstate_r(salted_seed, _statebuf, STATELEN, &_randbuf);
    srandom_r(salted_seed, &_randbuf);
}

}; // namespace chai_internal
