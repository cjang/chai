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
    // first allocate backing memory
    const size_t W = _argScalar[0];
    const size_t H = (2 == _N ? _argScalar[1] : 1);
    BackMem* backMem = allocBackMem(W, H, _precision);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W, H, _precision, backMem, i);

        frontMem.push_back(m);

        switch (_precision)
        {
            case (PrecType::UInt32) :
                for (size_t j = 0; j < W * H; j++)
                    m->uintPtr()[j] = _uintValue;
                break;

            case (PrecType::Int32) :
                for (size_t j = 0; j < W * H; j++)
                    m->intPtr()[j] = _intValue;
                break;

            case (PrecType::Float) :
                for (size_t j = 0; j < W * H; j++)
                    m->floatPtr()[j] = _floatValue;
                break;

            case (PrecType::Double) :
                for (size_t j = 0; j < W * H; j++)
                    m->doublePtr()[j] = _doubleValue;
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpLitdata::InterpLitdata(const uint32_t a, const size_t N)
    : BaseInterp(N, 0),
      _precision(PrecType::UInt32),
      _uintValue(a),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0),
      _N(N) { }

InterpLitdata::InterpLitdata(const int32_t a, const size_t N)
    : BaseInterp(N, 0),
      _precision(PrecType::UInt32),
      _uintValue(0),
      _intValue(a),
      _floatValue(0),
      _doubleValue(0),
      _N(N) { }

InterpLitdata::InterpLitdata(const float a, const size_t N)
    : BaseInterp(N, 0),
      _precision(PrecType::Float),
      _uintValue(0),
      _intValue(0),
      _floatValue(a),
      _doubleValue(0),
      _N(N) { }

InterpLitdata::InterpLitdata(const double a, const size_t N)
    : BaseInterp(N, 0),
      _precision(PrecType::Double),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(a),
      _N(N) { }

}; // namespace chai_internal
