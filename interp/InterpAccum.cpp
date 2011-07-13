// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpAccum.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// mean, sum

void InterpAccum::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    BackMem* backMem = allocBackMem(1, 1, isDouble(0));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(1, 1, isDouble(0), backMem, i);

        frontMem.push_back(m);

        double accum = 0;

        if (isDouble(0))
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                accum += doublePtr(0, i)[j];

            if (_takeAvg)
                accum /= (W(0) * H(0));

            m->doublePtr()[0] = accum;
        }
        else
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                accum += floatPtr(0, i)[j];

            if (_takeAvg)
                accum /= (W(0) * H(0));

            m->floatPtr()[0] = accum;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpAccum::InterpAccum(const bool takeAvg)
    : BaseInterp(0, 1),
      _takeAvg(takeAvg) { }

}; // namespace chai_internal
