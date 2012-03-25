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

    const size_t prec0 = precision(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(H(0), W(0), prec0);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(H(0), W(0), prec0, backMem, i);

        frontMem.push_back(m);

        switch (prec0)
        {
            case (PrecType::UInt32) :
                for (size_t x = 0; x < W(0); x++)
                for (size_t y = 0; y < H(0); y++)
                    m->uintPtr()[y + x * H(0)]
                        = uintPtr(0, i)[x + y * W(0)];
                break;

            case (PrecType::Int32) :
                for (size_t x = 0; x < W(0); x++)
                for (size_t y = 0; y < H(0); y++)
                    m->intPtr()[y + x * H(0)]
                        = intPtr(0, i)[x + y * W(0)];
                break;

            case (PrecType::Float) :
                for (size_t x = 0; x < W(0); x++)
                for (size_t y = 0; y < H(0); y++)
                    m->floatPtr()[y + x * H(0)]
                        = floatPtr(0, i)[x + y * W(0)];
                break;

            case (PrecType::Double) :
                for (size_t x = 0; x < W(0); x++)
                for (size_t y = 0; y < H(0); y++)
                    m->doublePtr()[y + x * H(0)]
                        = doublePtr(0, i)[x + y * W(0)];
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpTranspose::InterpTranspose(void)
    : BaseInterp(0, 1) { }

}; // namespace chai_internal
