// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpNOP.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// language extension interpreter object

void InterpNOP::sub_eval(std::stack< std::vector< FrontMem* > >& outStack)
{
    swizzle(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(W(0), H(0), isDouble(0));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W(0), H(0), isDouble(0), backMem, i);

        frontMem.push_back(m);

        if (isDouble(0))
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                m->doublePtr()[j] = doublePtr(0, i)[j];
        }
        else
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                m->floatPtr()[j] = floatPtr(0, i)[j];
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpNOP::InterpNOP(void)
    : BaseInterp(0, 1) { }

BaseInterp* InterpNOP::clone(void) const
{
    return new InterpNOP;
}

}; // namespace chai_internal
