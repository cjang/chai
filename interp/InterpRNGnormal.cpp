// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "InterpRNGnormal.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_f32
// rng_normal_make_f64

void InterpRNGnormal::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t variant    = _argScalar[0]; // not used
    const unsigned int seed = _argScalar[1]; // not used
    const size_t WIDTH      = _argScalar[2];
    const size_t HEIGHT     = _argScalar[3];
    const size_t SLOTS      = _argScalar[4];

    const size_t LEN        = WIDTH * HEIGHT;

    _gen.seed(random());

    // first allocate backing memory
    BackMem* backMem = allocBackMem(_prec, WIDTH, HEIGHT, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(_prec, WIDTH, HEIGHT, backMem, i);

        frontMem.push_back(m);

        switch (_prec)
        {
            case (PrecType::Float) :
                for (size_t j = 0; j < LEN; j++)
                    m->floatPtr()[j] = _gen.normal<float>();
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < LEN; j++)
                    m->doublePtr()[j] = _gen.normal<double>();
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGnormal::InterpRNGnormal(const size_t PREC, InterpRNG& gen)
    : BaseInterp(5, 0),
      _prec(PREC),
      _gen(gen) { }

}; // namespace chai_internal
