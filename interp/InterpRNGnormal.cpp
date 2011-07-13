// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "InterpRNGnormal.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_f32, rng_normal_make_f64

void InterpRNGnormal::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    //const unsigned int seed = 0;
    const unsigned int seed = random();
    const size_t variant    = _argScalar[0]; // not used
    const size_t len        = _argScalar[1];

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
                m->doublePtr()[j] = _gen.normal<double>();
        }
        else
        {
            for (size_t j = 0; j < len; j++)
                m->floatPtr()[j] = _gen.normal<float>();
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGnormal::InterpRNGnormal(const bool isDP, InterpRNG& gen)
    : BaseInterp(2, 0),
      _isDP(isDP),
      _gen(gen) { }

}; // namespace chai_internal
