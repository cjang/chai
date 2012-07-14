// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemInterp.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// callbacks from back memory

// data movement from compute device back to CPU
void MemInterp::swizzle(BackMem* backMem)
{
}

// called from the BackMem destructor
void MemInterp::untrackMem(BackMem* backMem)
{
}

}; // namespace chai_internal
