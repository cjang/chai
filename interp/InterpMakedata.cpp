// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMakedata.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// make1_u32, make1_i64
// make1_f32, make1_f64
// make2_u32, make2_i64
// make2_f32, make2_f64

void InterpMakedata::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    // ignoring the scalar width and height arguments
    // they are used only for bytecode hashing

    // array memory boxes
    vector< FrontMem* > frontMem;

    for (vector< FrontMem* >::const_iterator
         it = _argMem[0].begin();
         it != _argMem[0].end();
         it++)
    {
        frontMem.push_back(*it);
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpMakedata::InterpMakedata(void)
    : BaseInterp(3, 0) { }

}; // namespace chai_internal
