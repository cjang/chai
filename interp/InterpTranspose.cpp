// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpTranspose.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// transpose

void InterpTranspose::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    const size_t PREC   = prec(0);
    const size_t WIDTH  = W(0);
    const size_t HEIGHT = H(0);
    const size_t SLOTS  = slots(0); 

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, HEIGHT, WIDTH, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, HEIGHT, WIDTH, backMem, i);

        frontMem.push_back(m);

        switch (PREC)
        {
            case (PrecType::UInt32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->uintPtr()[y + x * HEIGHT]
                        = uintPtr(0, i)[x + y * WIDTH];
                break;

            case (PrecType::Int32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->intPtr()[y + x * HEIGHT]
                        = intPtr(0, i)[x + y * WIDTH];
                break;

            case (PrecType::Float) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->floatPtr()[y + x * HEIGHT]
                        = floatPtr(0, i)[x + y * WIDTH];
                break;

            case (PrecType::Double) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->doublePtr()[y + x * HEIGHT]
                        = doublePtr(0, i)[x + y * WIDTH];
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpTranspose::InterpTranspose(void)
    : BaseInterp(0, 1) { }

}; // namespace chai_internal
