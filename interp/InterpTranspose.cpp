// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpTranspose.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// transpose

void InterpTranspose::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(H(0), W(0), isDouble(0));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(H(0), W(0), isDouble(0), backMem, i);

        frontMem.push_back(m);

        if (isDouble(0))
        {
            for (size_t x = 0; x < W(0); x++)
            for (size_t y = 0; y < H(0); y++)
                m->doublePtr()[y + x * H(0)] = doublePtr(0, i)[x + y * W(0)];
        }
        else
        {
            for (size_t x = 0; x < W(0); x++)
            for (size_t y = 0; y < H(0); y++)
                m->floatPtr()[y + x * H(0)] = floatPtr(0, i)[x + y * W(0)];
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpTranspose::InterpTranspose(void)
    : BaseInterp(0, 1) { }

}; // namespace chai_internal
