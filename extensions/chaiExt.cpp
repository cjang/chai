// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "chaiExt.h"
#include "ByteCodesExt.hpp"
#include "InterpNOP.hpp"
#include "Scheduler.hpp"
#include "TransNOP.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// language extension API

void initExt(void)
{
    InterpNOP iNOP;
    TransNOP tNOP;
    Scheduler::singleton().extendLanguage(ByteCodes::nop, iNOP, tNOP);
}

ArrayFun nop(const ArrayExp& A)
{
    return ArrayFun(ByteCodes::nop, A, ArrayFun::UNCHANGED);
}

}; // namespace chai
