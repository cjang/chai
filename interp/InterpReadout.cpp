// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "InterpReadout.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read_scalar, read1, read2

void InterpReadout::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);

    if (isDouble(0))
    {
        for (size_t i = 0; i < numTraces(); i++)
            memcpy(_argMem[0][i]->doublePtr(), doublePtr(0, i), frontSize(0));
    }
    else
    {
        for (size_t i = 0; i < numTraces(); i++)
            memcpy(_argMem[0][i]->floatPtr(), floatPtr(0, i), frontSize(0));
    }
}

InterpReadout::InterpReadout(void)
    : BaseInterp(1, 1) { }

}; // namespace chai_internal
