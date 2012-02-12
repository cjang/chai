// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmul.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matmul

void InterpMatmul::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    doMatmul(outStack);
}

InterpMatmul::InterpMatmul(void)
    : InterpMatmulBase(0, 2) { }

}; // namespace chai_internal
