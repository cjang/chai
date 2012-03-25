// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpIdxdata.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// index1_u32, index1_i32
// index1_f32, index1_f64
// index2_u32, index2_i64
// index2_f32, index2_f64

void InterpIdxdata::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    const size_t index = _argScalar[0];
    const size_t W     = _argScalar[1];
    const size_t H     = (2 == _N ? _argScalar[2] : 1);
    BackMem* backMem = allocBackMem(W, H, _precision);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W, H, _precision, backMem, i);

        frontMem.push_back(m);

        if (0 == index)
        {
            switch (_precision)
            {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->uintPtr()[ x + y * H ] = x;
                    break;

                case (PrecType::Int32) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->intPtr()[ x + y * H ] = x;
                    break;

                case (PrecType::Float) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->floatPtr()[ x + y * H ] = x;
                    break;

                case (PrecType::Double) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->doublePtr()[ x + y * H ] = x;
                    break;
            }
        }
        else
        {
            switch (_precision)
            {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->uintPtr()[ x + y * H ] = y;
                    break;

                case (PrecType::Int32) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->intPtr()[ x + y * H ] = y;
                    break;

                case (PrecType::Float) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->floatPtr()[ x + y * H ] = y;
                    break;

                case (PrecType::Double) :
                    for (size_t x = 0; x < W; x++)
                    for (size_t y = 0; y < H; y++)
                        m->doublePtr()[ x + y * H ] = y;
                    break;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpIdxdata::InterpIdxdata(const size_t precision, const size_t N)
    : BaseInterp(N + 1, 0),
      _precision(precision),
      _N(N) { }

}; // namespace chai_internal
