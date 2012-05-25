// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "InterpRNGuniform.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_u32
// rng_uniform_make_i32
// rng_uniform_make_f32
// rng_uniform_make_f64

void InterpRNGuniform::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t variant    = _argScalar[0]; // not used
    const unsigned int seed = _argScalar[1]; // not used
    const size_t len        = _argScalar[2];
    const size_t step       = _argScalar[3];
    const double minlimit   = _argScalar[4];
    const double maxlimit   = _argScalar[5];

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
                    m->uintPtr()[j] = _gen.uniform<uint32_t>(
                                            step,
                                            static_cast<uint32_t>(minlimit),
                                            static_cast<uint32_t>(maxlimit));
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < len; j++)
                    m->intPtr()[j] = _gen.uniform<int32_t>(
                                            step,
                                            static_cast<int32_t>(minlimit),
                                            static_cast<int32_t>(maxlimit));
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < len; j++)
                    m->floatPtr()[j] = _gen.uniform<float>(
                                            step,
                                            static_cast<float>(minlimit),
                                            static_cast<float>(maxlimit));
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < len; j++)
                    m->doublePtr()[j] = _gen.uniform<double>(
                                            step,
                                            minlimit,
                                            maxlimit);
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGuniform::InterpRNGuniform(const size_t precision, InterpRNG& gen)
    : BaseInterp(6, 0),
      _precision(precision),
      _gen(gen) { }

}; // namespace chai_internal
