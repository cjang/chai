// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpLitdata.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

void InterpLitdata::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // first allocate backing memory
    const size_t W = _argScalar[0];
    const size_t H = (2 == _N ? _argScalar[1] : 1);
    BackMem* backMem = allocBackMem(W, H, _isDP);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W, H, _isDP, backMem, i);

        frontMem.push_back(m);

        if (_isDP)
        {
            for (size_t j = 0; j < W * H; j++)
                m->doublePtr()[j] = _doubleValue;
        }
        else
        {
            for (size_t j = 0; j < W * H; j++)
                m->floatPtr()[j] = _floatValue;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpLitdata::InterpLitdata(const float a, const size_t N)
    : BaseInterp(N, 0),
      _isDP(false),
      _floatValue(a),
      _doubleValue(0),
      _N(N) { }

InterpLitdata::InterpLitdata(const double a, const size_t N)
    : BaseInterp(N, 0),
      _isDP(true),
      _floatValue(0),
      _doubleValue(a),
      _N(N) { }

}; // namespace chai_internal
