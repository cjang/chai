// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpScalar.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// scalar_f32, scalar_f64

void InterpScalar::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const double scalar = _argScalar[0];

    // first allocate backing memory
    BackMem* backMem = allocBackMem(1, 1, _isDP);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(1, 1, _isDP, backMem, i);

        frontMem.push_back(m);

        if (_isDP)
        {
            m->doublePtr()[0] = scalar;
        }
        else
        {
            m->floatPtr()[0] = static_cast<float>(scalar);
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpScalar::InterpScalar(const bool isDP)
    : BaseInterp(1, 0),
      _isDP(isDP) { }

}; // namespace chai_internal
