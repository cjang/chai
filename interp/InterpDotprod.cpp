// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpDotprod.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dot_product

void InterpDotprod::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    BackMem* backMem = allocBackMem(1, 1, isDouble(0) || isDouble(1));

    // array memory boxes
    vector< FrontMem* > frontMem;

    const size_t maxW = max<size_t>(W(0), W(1));
    const size_t maxH = max<size_t>(H(0), H(1));

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(1,
                                    1,
                                    isDouble(0) || isDouble(1),
                                    backMem,
                                    i);

        frontMem.push_back(m);

        double accum = 0;

        if (isDouble(0))
        {
            if (isDouble(1))
            {
                for (size_t j = 0; j < maxW * maxH; j++)
                    accum += doublePtr(0, i)[j] * doublePtr(1, i)[j];

                m->doublePtr()[0] = accum;
            }
            else
            {
                for (size_t j = 0; j < maxW * maxH; j++)
                    accum += doublePtr(0, i)[j] * floatPtr(1, i)[j];

                m->doublePtr()[0] = accum;
            }

        }
        else
        {
            if (isDouble(1))
            {
                for (size_t j = 0; j < maxW * maxH; j++)
                    accum += floatPtr(0, i)[j] * doublePtr(1, i)[j];

                m->doublePtr()[0] = accum;
            }
            else
            {
                for (size_t j = 0; j < maxW * maxH; j++)
                    accum += floatPtr(0, i)[j] * floatPtr(1, i)[j];

                m->floatPtr()[0] = accum;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpDotprod::InterpDotprod(void)
    : BaseInterp(0, 2) { }

}; // namespace chai_internal
