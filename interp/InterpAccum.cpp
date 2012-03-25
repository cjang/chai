// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpAccum.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// mean, sum

void InterpAccum::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    const size_t prec0 = precision(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(1, 1, prec0);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(1, 1, prec0, backMem, i);

        frontMem.push_back(m);

        double accum = 0;

        switch (prec0)
        {
            case (PrecType::UInt32) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    accum += uintPtr(0, i)[j];
                if (_takeAvg) accum /= (W(0) * H(0));
                m->uintPtr()[0] = accum;
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    accum += intPtr(0, i)[j];
                if (_takeAvg) accum /= (W(0) * H(0));
                m->intPtr()[0] = accum;
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    accum += floatPtr(0, i)[j];
                if (_takeAvg) accum /= (W(0) * H(0));
                m->floatPtr()[0] = accum;
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    accum += doublePtr(0, i)[j];
                if (_takeAvg) accum /= (W(0) * H(0));
                m->doublePtr()[0] = accum;
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpAccum::InterpAccum(const bool takeAvg)
    : BaseInterp(0, 1),
      _takeAvg(takeAvg) { }

}; // namespace chai_internal
