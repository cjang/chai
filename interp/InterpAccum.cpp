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

    const size_t PREC  = prec(0);
    const size_t LEN   = W(0) * H(0);
    const size_t SLOTS = slots(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, 1, 1, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, 1, 1, backMem, i);

        frontMem.push_back(m);

        double accum = 0;

        switch (PREC)
        {
            case (PrecType::UInt32) :
                for (size_t j = 0; j < LEN; j++)
                    accum += uintPtr(0, i)[j];
                if (_takeAvg) accum /= LEN;
                m->uintPtr()[0] = accum;
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < LEN; j++)
                    accum += intPtr(0, i)[j];
                if (_takeAvg) accum /= LEN;
                m->intPtr()[0] = accum;
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < LEN; j++)
                    accum += floatPtr(0, i)[j];
                if (_takeAvg) accum /= LEN;
                m->floatPtr()[0] = accum;
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < LEN; j++)
                    accum += doublePtr(0, i)[j];
                if (_takeAvg) accum /= LEN;
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
