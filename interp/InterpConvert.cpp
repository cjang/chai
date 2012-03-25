// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpConvert.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// convert_f32, convert_f64

void InterpConvert::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t prec0 = precision(0);

    // precision matches, no conversion
    if (_precision == prec0)
    {
        checkout(0);
        outStack.push(_argStack[0]);
        return;
    }

    // must change precision

    swizzle(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(W(0), H(0), _precision);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W(0), H(0), _precision, backMem, i);

        frontMem.push_back(m);

        for (size_t j = 0; j < W(0) * H(0); j++)
        {
            double value;
            switch (prec0)
            {
                case (PrecType::UInt32) :
                    value = uintPtr(0, i)[j];
                    break;

                case (PrecType::Int32) :
                    value = intPtr(0, i)[j];
                    break;

                case (PrecType::Float) :
                    value = floatPtr(0, i)[j];
                    break;

                case (PrecType::Double) :
                    value = doublePtr(0, i)[j];
                    break;
            }

            switch (_precision)
            {
                case (PrecType::UInt32) :
                    m->uintPtr()[j] = value;
                    break;

                case (PrecType::Int32) :
                    m->intPtr()[j] = value;
                    break;

                case (PrecType::Float) :
                    m->floatPtr()[j] = value;
                    break;

                case (PrecType::Double) :
                    m->doublePtr()[j] = value;
                    break;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpConvert::InterpConvert(const size_t precision)
    : BaseInterp(0, 1),
      _precision(precision) { }

}; // namespace chai_internal
