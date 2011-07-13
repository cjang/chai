// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpConvert.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// convert_f32, convert_f64

void InterpConvert::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // precision matches, no conversion
    if (_isDP == isDouble(0))
    {
        checkout(0);
        outStack.push(_argStack[0]);
        return;
    }

    // must change precision

    // first allocate backing memory
    BackMem* backMem = allocBackMem(W(0), H(0), ! isDouble(0));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W(0), H(0), ! isDouble(0), backMem, i);

        frontMem.push_back(m);

        if (isDouble(0))
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                m->floatPtr()[j] = static_cast<float>(doublePtr(0, i)[j]);
        }
        else
        {
            for (size_t j = 0; j < W(0) * H(0); j++)
                m->doublePtr()[j] = static_cast<double>(floatPtr(0, i)[j]);
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpConvert::InterpConvert(const bool isDP)
    : BaseInterp(0, 1),
      _isDP(isDP) { }

}; // namespace chai_internal
