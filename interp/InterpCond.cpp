// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpCond.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// cond

void InterpCond::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    const size_t maxW = max<size_t>(W(1), W(2));
    const size_t maxH = max<size_t>(H(1), H(2));
    BackMem* backMem = allocBackMem(maxW,
                                    maxH,
                                    isDouble(1) || isDouble(2));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(maxW,
                                    maxH,
                                    isDouble(0) || isDouble(1),
                                    backMem,
                                    i);

        frontMem.push_back(m);

        for (size_t x = 0; x < maxW; x++)
        for (size_t y = 0; y < maxH; y++)
        {
            const size_t getIndex
                = ( (isDouble(0) && doublePtr(0, i)[idx(0, x, y)]) ||
                    (isFloat(0) && floatPtr(0, i)[idx(0, x, y)]) )
                      ? 1
                      : 2;

            const double value
                = isDouble(getIndex)
                      ? doublePtr(getIndex, i)[idx(getIndex, x, y)]
                      : floatPtr(getIndex, i)[idx(getIndex, x, y)];

            if (isDouble(1) || isDouble(2))
            {
                m->doublePtr()[ x + y * maxW ] = value;
            }
            else
            {
                m->floatPtr()[ x + y * maxW ] = value;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpCond::InterpCond(void)
    : BaseInterp(0, 3) { }

}; // namespace chai_internal
