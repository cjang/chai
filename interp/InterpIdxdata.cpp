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
    const size_t INDEX  = _argScalar[0];
    const size_t WIDTH  = _argScalar[1];
    const size_t HEIGHT = (2 == _N ? _argScalar[2] : 1);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(_prec, WIDTH, HEIGHT, 1);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    FrontMem* m = allocFrontMem(_prec, WIDTH, HEIGHT, backMem, 0);

    frontMem.push_back(m);

    if (0 == INDEX)
    {
        switch (_prec)
        {
            case (PrecType::UInt32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->uintPtr()[ x + y * WIDTH ] = x;
                break;

            case (PrecType::Int32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->intPtr()[ x + y * WIDTH ] = x;
                break;

            case (PrecType::Float) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->floatPtr()[ x + y * WIDTH ] = x;
                break;

            case (PrecType::Double) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->doublePtr()[ x + y * WIDTH ] = x;
                break;
        }
    }
    else
    {
        switch (_prec)
        {
            case (PrecType::UInt32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->uintPtr()[ x + y * WIDTH ] = y;
                break;

            case (PrecType::Int32) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->intPtr()[ x + y * WIDTH ] = y;
                break;

            case (PrecType::Float) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->floatPtr()[ x + y * WIDTH ] = y;
                break;

            case (PrecType::Double) :
                for (size_t x = 0; x < WIDTH; x++)
                for (size_t y = 0; y < HEIGHT; y++)
                    m->doublePtr()[ x + y * WIDTH ] = y;
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpIdxdata::InterpIdxdata(const size_t N, const size_t PREC)
    : BaseInterp(N + 1, 0),
      _N(N),
      _prec(PREC) { }

}; // namespace chai_internal
