// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpCond.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// cond

void InterpCond::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);
    swizzle(2);

    const size_t prec0  = prec(0);
    const size_t prec1  = prec(1);
    const size_t prec2  = prec(2);
    const size_t PREC   = max<size_t>(prec1, prec2);

    const size_t WIDTH  = max<size_t>(W(1), W(2));
    const size_t HEIGHT = max<size_t>(H(1), H(2));

    const size_t slots1 = slots(1);
    const size_t slots2 = slots(2);
    const size_t SLOTS  = slots1 < slots2 ? slots2 : slots1;

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

        frontMem.push_back(m);

        for (size_t x = 0; x < WIDTH; x++)
        for (size_t y = 0; y < HEIGHT; y++)
        {
            size_t getIndex;
            switch (prec0)
            {
                case (PrecType::UInt32) :
                    getIndex = uintPtr(0, i)[idx(0, x, y)] ? 1 : 2;
                    break;

                case (PrecType::Int32) :
                    getIndex = intPtr(0, i)[idx(0, x, y)] ? 1 : 2;
                    break;

                case (PrecType::Float) :
                    getIndex = floatPtr(0, i)[idx(0, x, y)] ? 1 : 2;
                    break;

                case (PrecType::Double) :
                    getIndex = doublePtr(0, i)[idx(0, x, y)] ? 1 : 2;
                    break;
            }

            double value;
            switch (prec(getIndex))
            {
                case (PrecType::UInt32) :
                    value = uintPtr(getIndex, i)[idx(getIndex, x, y)];
                    break;

                case (PrecType::Int32) :
                    value = intPtr(getIndex, i)[idx(getIndex, x, y)];
                    break;

                case (PrecType::Float) :
                    value = floatPtr(getIndex, i)[idx(getIndex, x, y)];
                    break;

                case (PrecType::Double) :
                    value = doublePtr(getIndex, i)[idx(getIndex, x, y)];
                    break;
            }

            switch (PREC)
            {
                case (PrecType::UInt32) :
                    m->uintPtr()[ x + y * WIDTH ] = value;
                    break;

                case (PrecType::Int32) :
                    m->intPtr()[ x + y * WIDTH ] = value;
                    break;

                case (PrecType::Float) :
                    m->floatPtr()[ x + y * WIDTH ] = value;
                    break;

                case (PrecType::Double) :
                    m->doublePtr()[ x + y * WIDTH ] = value;
                    break;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpCond::InterpCond(void)
    : BaseInterp(0, 3) { }

}; // namespace chai_internal
