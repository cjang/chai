// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpScalar.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// scalar_f32, scalar_f64

void InterpScalar::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const double scalar = _argScalar[0];

    // first allocate backing memory
    BackMem* backMem = allocBackMem(1, 1, _precision);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(1, 1, _precision, backMem, i);

        frontMem.push_back(m);

        switch (_precision)
        {
            case (PrecType::UInt32) :
                m->uintPtr()[0] = static_cast<uint32_t>(scalar);
                break;

            case (PrecType::Int32) :
                m->intPtr()[0] = static_cast<int32_t>(scalar);
                break;

            case (PrecType::Float) :
                m->floatPtr()[0] = static_cast<float>(scalar);
                break;

            case (PrecType::Double) :
                m->doublePtr()[0] = scalar;
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpScalar::InterpScalar(const size_t precision)
    : BaseInterp(1, 0),
      _precision(precision) { }

}; // namespace chai_internal
