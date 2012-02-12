// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmulG.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// general matmul

void InterpMatmulG::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    doMatmul(outStack);
}

InterpMatmulG::InterpMatmulG(void)
    : InterpMatmulBase(2, 3) { }

}; // namespace chai_internal
