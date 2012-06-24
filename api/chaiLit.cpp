// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array literals

#define DEFN_ARRAY_LIT(FP, X) \
\
ArrayLit ## FP :: ArrayLit ## FP (const X a) : _lit(a) { } \
\
X ArrayLit ## FP ::value(void) const { return _lit; } \
\
void ArrayLit ## FP ::accept(Stak<BC>& v) const { \
    v.push(static_cast<double>(_lit)); \
    v.push(ByteCodes::scalar_ ## FP); } \
\
void ArrayLit ## FP ::accept(stack< ArrayDim >& v) const { \
    v.push(ArrayDim(1, 1)); }

DEFN_ARRAY_LIT(u32, uint32_t)
DEFN_ARRAY_LIT(i32, int32_t)
DEFN_ARRAY_LIT(f32, float)
DEFN_ARRAY_LIT(f64, double)

}; // namespace chai
