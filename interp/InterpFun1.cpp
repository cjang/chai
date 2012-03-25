// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpFun1.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// one argument array stream functions

void InterpFun1::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    const size_t prec0 = precision(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(W(0), H(0), prec0);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W(0), H(0), prec0, backMem, i);

        frontMem.push_back(m);

        switch (prec0)
        {
            case (PrecType::UInt32) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    m->uintPtr()[j] = _fun(uintPtr(0, i)[j]);
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    m->intPtr()[j] = _fun(intPtr(0, i)[j]);
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    m->floatPtr()[j] = _fun(floatPtr(0, i)[j]);
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < W(0) * H(0); j++)
                    m->doublePtr()[j] = _fun(doublePtr(0, i)[j]);
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpFun1::InterpFun1(const SimpleFun1& f)
    : BaseInterp(0, 1),
      _fun(f) { }

}; // namespace chai_internal
