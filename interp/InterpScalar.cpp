// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpScalar.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// scalar_f32, scalar_f64

void InterpScalar::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    const double SCALAR = _argScalar[0];

    // first allocate backing memory
    BackMem* backMem = allocBackMem(_prec, 1, 1, 1);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    FrontMem* m = allocFrontMem(_prec, 1, 1, backMem, 0);

    frontMem.push_back(m);

    switch (_prec)
    {
        case (PrecType::UInt32) :
            m->uintPtr()[0] = static_cast<uint32_t>(SCALAR);
            break;

        case (PrecType::Int32) :
            m->intPtr()[0] = static_cast<int32_t>(SCALAR);
            break;

        case (PrecType::Float) :
            m->floatPtr()[0] = static_cast<float>(SCALAR);
            break;

        case (PrecType::Double) :
            m->doublePtr()[0] = SCALAR;
            break;
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpScalar::InterpScalar(const size_t PREC)
    : BaseInterp(1, 0),
      _prec(PREC) { }

}; // namespace chai_internal
