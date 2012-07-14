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

size_t InterpRNGuniform::numberArgs(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
            return 5;

        case (PrecType::Float) :
        case (PrecType::Double) :
            return 7;
    }
}

void InterpRNGuniform::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t variant    = _argScalar[0]; // not used
    const unsigned int seed = _argScalar[1]; // not used
    const size_t WIDTH      = _argScalar[2];
    const size_t HEIGHT     = _argScalar[3];
    const size_t SLOTS      = _argScalar[4];
    const double minLimit   = _hasLimits ? _argScalar[5] : 0;
    const double maxLimit   = _hasLimits ? _argScalar[6] : 0;

    const size_t LEN        = WIDTH * HEIGHT;

    const size_t STEP = 1;

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
            case (PrecType::UInt32) :
                for (size_t j = 0; j < LEN; j++)
                    m->uintPtr()[j] = _gen.uniform<uint32_t>(STEP);
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < LEN; j++)
                    m->intPtr()[j] = _gen.uniform<int32_t>(STEP);
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < LEN; j++)
                    m->floatPtr()[j] = _gen.uniform<float>(
                                           STEP,
                                           static_cast<float>(minLimit),
                                           static_cast<float>(maxLimit));
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < LEN; j++)
                    m->doublePtr()[j] = _gen.uniform<double>(
                                            STEP,
                                            minLimit,
                                            maxLimit);
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpRNGuniform::InterpRNGuniform(const size_t PREC, InterpRNG& gen)
    : BaseInterp(numberArgs(PREC), 0),
      _prec(PREC),
      _gen(gen),
      _hasLimits(7 == numberArgs(PREC)) { }

}; // namespace chai_internal
