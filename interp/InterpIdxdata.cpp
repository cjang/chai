// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpIdxdata.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// index1_f32, index1_f64, index2_f32, index2_f64

void InterpIdxdata::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    const size_t index = _argScalar[0];
    const size_t W     = _argScalar[1];
    const size_t H     = (2 == _N ? _argScalar[2] : 1);
    BackMem* backMem = allocBackMem(W, H, _isDP);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W, H, _isDP, backMem, i);

        frontMem.push_back(m);

        if (0 == index)
        {
            if (_isDP)
            {
                for (size_t x = 0; x < W; x++)
                for (size_t y = 0; y < H; y++)
                    m->doublePtr()[ x + y * H ] = x;
            }
            else
            {
                for (size_t x = 0; x < W; x++)
                for (size_t y = 0; y < H; y++)
                    m->floatPtr()[ x + y * H ] = x;
            }
        }
        else
        {
            if (_isDP)
            {
                for (size_t x = 0; x < W; x++)
                for (size_t y = 0; y < H; y++)
                    m->doublePtr()[ x + y * H ] = y;
            }
            else
            {
                for (size_t x = 0; x < W; x++)
                for (size_t y = 0; y < H; y++)
                    m->floatPtr()[ x + y * H ] = y;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpIdxdata::InterpIdxdata(const bool isDP, const size_t N)
    : BaseInterp(N + 1, 0),
      _isDP(isDP),
      _N(N) { }

}; // namespace chai_internal
