// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array operation with two arguments

#define DEFN_ARRAY_FUN2(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN2(dot_product, dot_product, REDUCED)
DEFN_ARRAY_FUN2(gather1_floor, gather1_floor, FIRST2)
DEFN_ARRAY_FUN2(isordered, isordered, FIRST2)
DEFN_ARRAY_FUN2(isunordered, isunordered, FIRST2)
DEFN_ARRAY_FUN2(matmul, matmul, MATMUL)

}; // namespace chai
