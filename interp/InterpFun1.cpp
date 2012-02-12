// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpFun1.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// one argument array stream functions

void InterpFun1::sub_eval(stack< vector< FrontMem* > >& outStack)
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
                m->doublePtr()[j] = _fun(doublePtr(0, i)[j]);
        }
        else
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                m->floatPtr()[j] = _fun(floatPtr(0, i)[j]);
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpFun1::InterpFun1(const SimpleFun1& f)
    : BaseInterp(0, 1),
      _fun(f) { }

}; // namespace chai_internal
