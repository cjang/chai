// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "InterpRNGuniform.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_f32, rng_uniform_make_f64

void InterpRNGuniform::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    //const unsigned int seed = 0;
    const unsigned int seed = random();
    const size_t variant    = _argScalar[0]; // not used
    const size_t len        = _argScalar[1];
    const size_t step       = _argScalar[2];
    const double minlimit   = _argScalar[3];
    const double maxlimit   = _argScalar[4];

    _gen.seed(seed);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(len, 1, _isDP);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(len, 1, _isDP, backMem, i);

        frontMem.push_back(m);

        if (_isDP)
        {
            for (size_t j = 0; j < len; j++)
                m->doublePtr()[j] = _gen.uniform<double>(
                                        step,
                                        minlimit,
                                        maxlimit);
        }
        else
        {
            for (size_t j = 0; j < len; j++)
                m->floatPtr()[j] = _gen.uniform<float>(
                                        step,
                                        static_cast<float>(minlimit),
                                        static_cast<float>(maxlimit));
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGuniform::InterpRNGuniform(const bool isDP, InterpRNG& gen)
    : BaseInterp(5, 0),
      _isDP(isDP),
      _gen(gen) { }

}; // namespace chai_internal
