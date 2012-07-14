// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <stdlib.h>

#include "SimpleFun1.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// unary functions

SimpleFun1::~SimpleFun1(void) { }

uint32_t SimpleFun1::operator() (const uint32_t) const
{
    return 0;
}

int32_t SimpleFun1::operator() (const int32_t) const
{
    return 0;
}

float SimpleFun1::operator() (const float) const
{
    return 0;
}

double SimpleFun1::operator() (const double) const
{
    return 0;
}

#define DEFN_SIMPLE_FUN_FP1(X, FUN32, FUN64) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
float _ ## X ## _ ::operator() (const float a) const { \
    return FUN32 (a); } \
\
double _ ## X ## _ ::operator() (const double a) const { \
    return FUN64 (a); }

_ABS_ & _ABS_::s(void)
{
    static _ABS_ a;
    return a;
}
int32_t _ABS_::operator() (const int32_t a) const
{
    return abs(a);
}
uint32_t _ABS_::operator() (const uint32_t a) const
{
    return a;
}

DEFN_SIMPLE_FUN_FP1(ACOS, acosf, acos)
DEFN_SIMPLE_FUN_FP1(ACOSH, acoshf, acosh)

_ACOSPI_ & _ACOSPI_::s(void)
{
    static _ACOSPI_ a;
    return a;
}
float _ACOSPI_::operator() (const float a) const
{
    return acosf(a) / M_PI;
}
double _ACOSPI_::operator() (const double a) const
{
    return acos(a) / M_PI;
}

DEFN_SIMPLE_FUN_FP1(ASIN, asinf, asin)
DEFN_SIMPLE_FUN_FP1(ASINH, asinhf, asinh)

_ASINPI_ & _ASINPI_::s(void)
{
    static _ASINPI_ a;
    return a;
}
float _ASINPI_::operator() (const float a) const
{
    return asinf(a) / M_PI;
}
double _ASINPI_::operator() (const double a) const
{
    return asin(a) / M_PI;
}

DEFN_SIMPLE_FUN_FP1(ATAN, atanf, atan)
DEFN_SIMPLE_FUN_FP1(ATANH, atanhf, atanh)

_ATANPI_ & _ATANPI_::s(void)
{
    static _ATANPI_ a;
    return a;
}
float _ATANPI_::operator() (const float a) const
{
    return atanf(a) / M_PI;
}
double _ATANPI_::operator() (const double a) const
{
    return atan(a) / M_PI;
}

DEFN_SIMPLE_FUN_FP1(CBRT, cbrtf, cbrt)
DEFN_SIMPLE_FUN_FP1(CEIL, ceilf, ceil)

_CLZ_ & _CLZ_::s(void)
{
    static _CLZ_ a;
    return a;
}
int32_t _CLZ_::operator() (const int32_t a) const
{
    int32_t b = a, count = 0;
    for (int i = 0; i <= 8*sizeof(int32_t); i++)
    {
        if (b) count++;
        b >>= 1;
    }
    return 8*sizeof(int32_t) - count;
}
uint32_t _CLZ_::operator() (const uint32_t a) const
{
    uint32_t b = a, count = 0;
    for (int i = 0; i <= 8*sizeof(uint32_t); i++)
    {
        if (b) count++;
        b >>= 1;
    }
    return 8*sizeof(uint32_t) - count;
}

_COMPLEMENT_ & _COMPLEMENT_::s(void)
{
    static _COMPLEMENT_ a;
    return a;
}
int32_t _COMPLEMENT_::operator() (const int32_t a) const
{
    return ~ a;
}
uint32_t _COMPLEMENT_::operator() (const uint32_t a) const
{
    return ~ a;
}

DEFN_SIMPLE_FUN_FP1(COS, cosf, cos)
DEFN_SIMPLE_FUN_FP1(COSH, coshf, cosh)

_COSPI_ & _COSPI_::s(void)
{
    static _COSPI_ a;
    return a;
}
float _COSPI_::operator() (const float a) const
{
    return cosf(M_PI * a);
}
double _COSPI_::operator() (const double a) const
{
    return cos(M_PI * a);
}

_DEGREES_ & _DEGREES_::s(void)
{
    static _DEGREES_ a;
    return a;
}
float _DEGREES_::operator() (const float a) const
{
    return M_1_PI * 180 * a;
}
double _DEGREES_::operator() (const double a) const
{
    return M_1_PI * 180 * a;
}

DEFN_SIMPLE_FUN_FP1(ERFC, erfcf, erfc)
DEFN_SIMPLE_FUN_FP1(ERF, erff, erf)
DEFN_SIMPLE_FUN_FP1(EXP, expf, exp)
DEFN_SIMPLE_FUN_FP1(EXP2, exp2f, exp2)
DEFN_SIMPLE_FUN_FP1(EXP10, exp10f, exp10)
DEFN_SIMPLE_FUN_FP1(EXPM1, expm1f, expm1)
DEFN_SIMPLE_FUN_FP1(FABS, fabsf, fabs)

_FLOOR_ & _FLOOR_::s(void)
{
    static _FLOOR_ a;
    return a;
}
float _FLOOR_::operator() (const float a) const
{
    return floorf(a);
}
double _FLOOR_::operator() (const double a) const
{
    return floor(a);
}
int32_t _FLOOR_::operator() (const int32_t a) const
{
    return a;
}
uint32_t _FLOOR_::operator() (const uint32_t a) const
{
    return a;
}

DEFN_SIMPLE_FUN_FP1(ILOGB, ilogbf, ilogb)

_ISFINITE_ & _ISFINITE_::s(void)
{
    static _ISFINITE_ a;
    return a;
}
float _ISFINITE_::operator() (const float a) const
{
    return isfinite(a);
}
double _ISFINITE_::operator() (const double a) const
{
    return isfinite(a);
}

_ISINF_ & _ISINF_::s(void)
{
    static _ISINF_ a;
    return a;
}
float _ISINF_::operator() (const float a) const
{
    return isinf(a);
}
double _ISINF_::operator() (const double a) const
{
    return isinf(a);
}

_ISNAN_ & _ISNAN_::s(void)
{
    static _ISNAN_ a;
    return a;
}
float _ISNAN_::operator() (const float a) const
{
    return isnan(a);
}
double _ISNAN_::operator() (const double a) const
{
    return isnan(a);
}

_ISNORMAL_ & _ISNORMAL_::s(void)
{
    static _ISNORMAL_ a;
    return a;
}
float _ISNORMAL_::operator() (const float a) const
{
    return isnormal(a);
}
double _ISNORMAL_::operator() (const double a) const
{
    return isnormal(a);
}

DEFN_SIMPLE_FUN_FP1(LGAMMA, lgammaf, lgamma)
DEFN_SIMPLE_FUN_FP1(LOG, logf, log)
DEFN_SIMPLE_FUN_FP1(LOG2, log2f, log2)
DEFN_SIMPLE_FUN_FP1(LOG10, log10f, log10)
DEFN_SIMPLE_FUN_FP1(LOG1P, log1pf, log1p)
DEFN_SIMPLE_FUN_FP1(LOGB, logbf, logb)

_NAN_ & _NAN_::s(void)
{
    static _NAN_ a;
    return a;
}
float _NAN_::operator() (const float a) const
{
    return nan("");
}
double _NAN_::operator() (const double a) const
{
    return nan("");
}

_NOT_ & _NOT_::s(void)
{
    static _NOT_ a;
    return a;
}
int32_t _NOT_::operator() (const int32_t a) const
{
    return ! a;
}
uint32_t _NOT_::operator() (const uint32_t a) const
{
    return ! a;
}

_RADIANS_ & _RADIANS_::s(void)
{
    static _RADIANS_ a;
    return a;
}
float _RADIANS_::operator() (const float a) const
{
    return a * M_PI / 180;
}
double _RADIANS_::operator() (const double a) const
{
    return a * M_PI / 180;
}

DEFN_SIMPLE_FUN_FP1(RINT, rintf, rint)
DEFN_SIMPLE_FUN_FP1(ROUND, roundf, round)

_RSQRT_ & _RSQRT_::s(void)
{
    static _RSQRT_ a;
    return a;
}
float _RSQRT_::operator() (const float a) const
{
    return 1.f / sqrtf(a);
}
double _RSQRT_::operator() (const double a) const
{
    return 1.f / sqrt(a);
}

_SIGN_ & _SIGN_::s(void)
{
    static _SIGN_ a;
    return a;
}
float _SIGN_::operator() (const float a) const
{
    if (a > 0.0)
        return 1.0;
    else if (a < 0.0)
        return -1.0;
    else
        return a; // a must be -0.0 or +0.0
}
double _SIGN_::operator() (const double a) const
{
    if (a > 0.0)
        return 1.0;
    else if (a < 0.0)
        return -1.0;
    else
        return a; // a must be -0.0 or +0.0
}

DEFN_SIMPLE_FUN_FP1(SIGNBIT, signbit, signbit)
DEFN_SIMPLE_FUN_FP1(SIN, sinf, sin)
DEFN_SIMPLE_FUN_FP1(SINH, sinhf, sinh)

_SINPI_ & _SINPI_::s(void)
{
    static _SINPI_ a;
    return a;
}
float _SINPI_::operator() (const float a) const
{
    return sinf(M_PI * a);
}
double _SINPI_::operator() (const double a) const
{
    return sin(M_PI * a);
}

DEFN_SIMPLE_FUN_FP1(SQRT, sqrtf, sqrt)
DEFN_SIMPLE_FUN_FP1(TAN, tanf, tan)
DEFN_SIMPLE_FUN_FP1(TANH, tanhf, tanh)

_TANPI_ & _TANPI_::s(void)
{
    static _TANPI_ a;
    return a;
}
float _TANPI_::operator() (const float a) const
{
    return tanf(M_PI * a);
}
double _TANPI_::operator() (const double a) const
{
    return tan(M_PI * a);
}

DEFN_SIMPLE_FUN_FP1(TGAMMA, tgammaf, tgamma)
DEFN_SIMPLE_FUN_FP1(TRUNC, truncf, trunc)

_UNARYMINUS_ & _UNARYMINUS_::s(void)
{
    static _UNARYMINUS_ a;
    return a;
}
float _UNARYMINUS_::operator() (const float a) const
{
    return (- a);
}
double _UNARYMINUS_::operator() (const double a) const
{
    return (- a);
}
int32_t _UNARYMINUS_::operator() (const int32_t a) const
{
    return (- a);
}

}; // namespace chai_internal
