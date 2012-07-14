// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpLitdata.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// ones1_u32, ones1_i64
// ones1_f32, ones1_f64
// ones2_u32, ones2_i64
// ones2_f32, ones2_f64
// zeros1_u32, zeros1_i64
// zeros1_f32, zeros1_f64
// zeros2_u32, zeros2_i64
// zeros2_f32, zeros2_f64

void InterpLitdata::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const size_t WIDTH  = _argScalar[0];
    const size_t HEIGHT = (2 == _N ? _argScalar[1] : 1);
    const size_t LEN    = WIDTH * HEIGHT;

    // first allocate backing memory
    BackMem* backMem = allocBackMem(_prec, WIDTH, HEIGHT, 1);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    FrontMem* m = allocFrontMem(_prec, WIDTH, HEIGHT, backMem, 0);

    frontMem.push_back(m);

    switch (_prec)
    {
        case (PrecType::UInt32) :
            for (size_t j = 0; j < LEN; j++)
                m->uintPtr()[j] = _uintValue;
            break;

        case (PrecType::Int32) :
            for (size_t j = 0; j < LEN; j++)
                m->intPtr()[j] = _intValue;
            break;

        case (PrecType::Float) :
            for (size_t j = 0; j < LEN; j++)
                m->floatPtr()[j] = _floatValue;
            break;

        case (PrecType::Double) :
            for (size_t j = 0; j < LEN; j++)
                m->doublePtr()[j] = _doubleValue;
            break;
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpLitdata::InterpLitdata(const size_t N, const uint32_t a)
    : BaseInterp(N, 0),
      _prec(PrecType::UInt32),
      _N(N),
      _uintValue(a),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0) { }

InterpLitdata::InterpLitdata(const size_t N, const int32_t a)
    : BaseInterp(N, 0),
      _prec(PrecType::UInt32),
      _N(N),
      _uintValue(0),
      _intValue(a),
      _floatValue(0),
      _doubleValue(0) { }

InterpLitdata::InterpLitdata(const size_t N, const float a)
    : BaseInterp(N, 0),
      _prec(PrecType::Float),
      _N(N),
      _uintValue(0),
      _intValue(0),
      _floatValue(a),
      _doubleValue(0) { }

InterpLitdata::InterpLitdata(const size_t N, const double a)
    : BaseInterp(N, 0),
      _prec(PrecType::Double),
      _N(N),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(a) { }

}; // namespace chai_internal
