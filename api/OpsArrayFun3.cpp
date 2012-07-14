// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array operation with three arguments

#define DEFN_ARRAY_FUN3(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B, const ArrayExp& C) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, C, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN3(clamp, clamp, FIRST3)
DEFN_ARRAY_FUN3(fma, fma, FIRST3)
DEFN_ARRAY_FUN3(gather2_floor, gather2_floor, FIRST3)
DEFN_ARRAY_FUN3(mad, mad, FIRST3)
DEFN_ARRAY_FUN3(mad24, mad24, FIRST3)
DEFN_ARRAY_FUN3(mad_hi, mad_hi, FIRST3)
DEFN_ARRAY_FUN3(mad_sat, mad_sat, FIRST3)
DEFN_ARRAY_FUN3(mix, mix, FIRST3)
DEFN_ARRAY_FUN3(smoothstep, smoothstep, FIRST3)

}; // namespace chai
