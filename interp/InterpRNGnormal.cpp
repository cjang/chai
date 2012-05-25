// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "InterpRNGnormal.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// rng_normal_make_u32
// rng_normal_make_i32
// rng_normal_make_f32
// rng_normal_make_f64

void InterpRNGnormal::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t variant    = _argScalar[0]; // not used
    const unsigned int seed = _argScalar[1]; // not used
    const size_t len        = _argScalar[2];

    _gen.seed(random());

    // first allocate backing memory
    BackMem* backMem = allocBackMem(len, 1, _precision);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(len, 1, _precision, backMem, i);

        frontMem.push_back(m);

        switch (_precision)
        {
            case (PrecType::UInt32) :
                for (size_t j = 0; j < len; j++)
                    m->uintPtr()[j] = _gen.normal<uint32_t>();
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < len; j++)
                    m->intPtr()[j] = _gen.normal<int32_t>();
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < len; j++)
                    m->floatPtr()[j] = _gen.normal<float>();
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < len; j++)
                    m->doublePtr()[j] = _gen.normal<double>();
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGnormal::InterpRNGnormal(const size_t precision, InterpRNG& gen)
    : BaseInterp(3, 0),
      _precision(precision),
      _gen(gen) { }

}; // namespace chai_internal
