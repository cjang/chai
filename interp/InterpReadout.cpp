// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "InterpReadout.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read_scalar, read1, read2

void InterpReadout::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    const size_t LEN   = frontSize(0);
    const size_t SLOTS = slots(0);

    switch (prec(0))
    {
        case (PrecType::UInt32) :
            for (size_t i = 0; i < SLOTS; i++)
                memcpy(_argMem[0][i]->uintPtr(), uintPtr(0, i), LEN);
            break;

        case (PrecType::Int32) :
            for (size_t i = 0; i < SLOTS; i++)
                memcpy(_argMem[0][i]->intPtr(), intPtr(0, i), LEN);
            break;

        case (PrecType::Float) :
            for (size_t i = 0; i < SLOTS; i++)
                memcpy(_argMem[0][i]->floatPtr(), floatPtr(0, i), LEN);
            break;

        case (PrecType::Double) :
            for (size_t i = 0; i < SLOTS; i++)
                memcpy(_argMem[0][i]->doublePtr(), doublePtr(0, i), LEN);
            break;
    }
}

InterpReadout::InterpReadout(void)
    : BaseInterp(1, 1) { }

}; // namespace chai_internal
