// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array operation with one argument

#define DEFN_ARRAY_FUN1(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN1(abs, abs, UNCHANGED)
DEFN_ARRAY_FUN1(acos, acos, UNCHANGED)
DEFN_ARRAY_FUN1(acosh, acosh, UNCHANGED)
DEFN_ARRAY_FUN1(acospi, acospi, UNCHANGED)
DEFN_ARRAY_FUN1(asin, asin, UNCHANGED)
DEFN_ARRAY_FUN1(asinh, asinh, UNCHANGED)
DEFN_ARRAY_FUN1(asinpi, asinpi, UNCHANGED)
DEFN_ARRAY_FUN1(atan, atan, UNCHANGED)
DEFN_ARRAY_FUN1(atanh, atanh, UNCHANGED)
DEFN_ARRAY_FUN1(atanpi, atanpi, UNCHANGED)
DEFN_ARRAY_FUN1(cbrt, cbrt, UNCHANGED)
DEFN_ARRAY_FUN1(ceil, ceil, UNCHANGED)
DEFN_ARRAY_FUN1(clz, clz, UNCHANGED)
DEFN_ARRAY_FUN1(cos, cos, UNCHANGED)
DEFN_ARRAY_FUN1(cosh, cosh, UNCHANGED)
DEFN_ARRAY_FUN1(cospi, cospi, UNCHANGED)
DEFN_ARRAY_FUN1(degrees, degrees, UNCHANGED)
DEFN_ARRAY_FUN1(erfc, erfc, UNCHANGED)
DEFN_ARRAY_FUN1(erf, erf, UNCHANGED)
DEFN_ARRAY_FUN1(exp, exp, UNCHANGED)
DEFN_ARRAY_FUN1(exp2, exp2, UNCHANGED)
DEFN_ARRAY_FUN1(exp10, exp10, UNCHANGED)
DEFN_ARRAY_FUN1(expm1, expm1, UNCHANGED)
DEFN_ARRAY_FUN1(fabs, fabs, UNCHANGED)
DEFN_ARRAY_FUN1(floor, floor, UNCHANGED)
DEFN_ARRAY_FUN1(ilogb, ilogb, UNCHANGED)
DEFN_ARRAY_FUN1(isfinite, isfinite, UNCHANGED)
DEFN_ARRAY_FUN1(isinf, isinf, UNCHANGED)
DEFN_ARRAY_FUN1(isnan, isnan, UNCHANGED)
DEFN_ARRAY_FUN1(isnormal, isnormal, UNCHANGED)
DEFN_ARRAY_FUN1(lgamma, lgamma, UNCHANGED)
DEFN_ARRAY_FUN1(log, log, UNCHANGED)
DEFN_ARRAY_FUN1(log2, log2, UNCHANGED)
DEFN_ARRAY_FUN1(log10, log10, UNCHANGED)
DEFN_ARRAY_FUN1(log1p, log1p, UNCHANGED)
DEFN_ARRAY_FUN1(logb, logb, UNCHANGED)
DEFN_ARRAY_FUN1(mean, mean, REDUCED)
DEFN_ARRAY_FUN1(nan, nan, UNCHANGED)
DEFN_ARRAY_FUN1(operator-, operatorUNARYMINUS, UNCHANGED)
DEFN_ARRAY_FUN1(operator~, operatorCOMPLEMENT, UNCHANGED)
DEFN_ARRAY_FUN1(operator!, operatorNOT, UNCHANGED)
DEFN_ARRAY_FUN1(radians, radians, UNCHANGED)
DEFN_ARRAY_FUN1(rint, rint, UNCHANGED)
DEFN_ARRAY_FUN1(round, round, UNCHANGED)
DEFN_ARRAY_FUN1(rsqrt, rsqrt, UNCHANGED)
DEFN_ARRAY_FUN1(sign, sign, UNCHANGED)
DEFN_ARRAY_FUN1(signbit, signbit, UNCHANGED)
DEFN_ARRAY_FUN1(sin, sin, UNCHANGED)
DEFN_ARRAY_FUN1(sinh, sinh, UNCHANGED)
DEFN_ARRAY_FUN1(sinpi, sinpi, UNCHANGED)
DEFN_ARRAY_FUN1(sqrt, sqrt, UNCHANGED)
DEFN_ARRAY_FUN1(sum, sum, REDUCED)
DEFN_ARRAY_FUN1(tan, tan, UNCHANGED)
DEFN_ARRAY_FUN1(tanh, tanh, UNCHANGED)
DEFN_ARRAY_FUN1(tanpi, tanpi, UNCHANGED)
DEFN_ARRAY_FUN1(tgamma, tgamma, UNCHANGED)
DEFN_ARRAY_FUN1(transpose, transpose, TRANSPOSED)
DEFN_ARRAY_FUN1(trunc, trunc, UNCHANGED)

}; // namespace chai
