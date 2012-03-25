// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <stdlib.h>

#include "SimpleFuns.hpp"

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

#define DEFN_SIMPLE_FUN_FP1(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a) const { return FUN32 (a); } \
double _ ## X ## _ ::operator() (const double a) const { return FUN64 (a); }

_ABS_ & _ABS_::singleton(void) { static _ABS_ a; return a; }
string _ABS_::str(void) const { return "abs"; }
int32_t _ABS_::operator() (const int32_t a) const { return abs(a); }
uint32_t _ABS_::operator() (const uint32_t a) const { return a; }

DEFN_SIMPLE_FUN_FP1(ACOS, acos, acosf, acos)
DEFN_SIMPLE_FUN_FP1(ACOSH, acosh, acoshf, acosh)

_ACOSPI_ & _ACOSPI_::singleton(void) { static _ACOSPI_ a; return a; }
string _ACOSPI_::str(void) const { return "acospi"; }
float _ACOSPI_::operator() (const float a) const { return acosf(a) / M_PI; }
double _ACOSPI_::operator() (const double a) const { return acos(a) / M_PI; }

DEFN_SIMPLE_FUN_FP1(ASIN, asin, asinf, asin)
DEFN_SIMPLE_FUN_FP1(ASINH, asinh, asinhf, asinh)

_ASINPI_ & _ASINPI_::singleton(void) { static _ASINPI_ a; return a; }
string _ASINPI_::str(void) const { return "asinpi"; }
float _ASINPI_::operator() (const float a) const { return asinf(a) / M_PI; }
double _ASINPI_::operator() (const double a) const { return asin(a) / M_PI; }

DEFN_SIMPLE_FUN_FP1(ATAN, atan, atanf, atan)
DEFN_SIMPLE_FUN_FP1(ATANH, atanh, atanhf, atanh)

_ATANPI_ & _ATANPI_::singleton(void) { static _ATANPI_ a; return a; }
string _ATANPI_::str(void) const { return "atanpi"; }
float _ATANPI_::operator() (const float a) const { return atanf(a) / M_PI; }
double _ATANPI_::operator() (const double a) const { return atan(a) / M_PI; }

DEFN_SIMPLE_FUN_FP1(CBRT, cbrt, cbrtf, cbrt)
DEFN_SIMPLE_FUN_FP1(CEIL, ceil, ceilf, ceil)

_CLZ_ & _CLZ_::singleton(void) { static _CLZ_ a; return a; }
string _CLZ_::str(void) const { return "clz"; }
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

_COMPLEMENT_ & _COMPLEMENT_::singleton(void)
{
    static _COMPLEMENT_ a;
    return a;
}
string _COMPLEMENT_::str(void) const { return "~"; }
int32_t _COMPLEMENT_::operator() (const int32_t a) const { return ~ a; }
uint32_t _COMPLEMENT_::operator() (const uint32_t a) const { return ~ a; }

DEFN_SIMPLE_FUN_FP1(COS, cos, cosf, cos)
DEFN_SIMPLE_FUN_FP1(COSH, cosh, coshf, cosh)

_COSPI_ & _COSPI_::singleton(void) { static _COSPI_ a; return a; }
string _COSPI_::str(void) const { return "cospi"; }
float _COSPI_::operator() (const float a) const { return cosf(M_PI * a); }
double _COSPI_::operator() (const double a) const { return cos(M_PI * a); }

_DEGREES_ & _DEGREES_::singleton(void) { static _DEGREES_ a; return a; }
string _DEGREES_::str(void) const { return "degrees"; }
float _DEGREES_::operator() (const float a) const { return M_1_PI * 180 * a; }
double _DEGREES_::operator() (const double a) const { return M_1_PI * 180 * a; }

DEFN_SIMPLE_FUN_FP1(ERFC, erfc, erfcf, erfc)
DEFN_SIMPLE_FUN_FP1(ERF, erf, erff, erf)
DEFN_SIMPLE_FUN_FP1(EXP, exp, expf, exp)
DEFN_SIMPLE_FUN_FP1(EXP2, exp2, exp2f, exp2)
DEFN_SIMPLE_FUN_FP1(EXP10, exp10, exp10f, exp10)
DEFN_SIMPLE_FUN_FP1(EXPM1, expm1, expm1f, expm1)
DEFN_SIMPLE_FUN_FP1(FABS, fabs, fabsf, fabs)

_FLOOR_ & _FLOOR_::singleton(void) { static _FLOOR_ a; return a; }
string _FLOOR_::str(void) const { return "floor"; }
float _FLOOR_::operator() (const float a) const { return floorf(a); }
double _FLOOR_::operator() (const double a) const { return floor(a); }
int32_t _FLOOR_::operator() (const int32_t a) const { return a; }
uint32_t _FLOOR_::operator() (const uint32_t a) const { return a; }

DEFN_SIMPLE_FUN_FP1(ILOGB, ilogb, ilogbf, ilogb)

_ISFINITE_ & _ISFINITE_::singleton(void) { static _ISFINITE_ a; return a; }
string _ISFINITE_::str(void) const { return "isfinite"; }
float _ISFINITE_::operator() (const float a) const { return isfinite(a); }
double _ISFINITE_::operator() (const double a) const { return isfinite(a); }

_ISINF_ & _ISINF_::singleton(void) { static _ISINF_ a; return a; }
string _ISINF_::str(void) const { return "isinf"; }
float _ISINF_::operator() (const float a) const { return isinf(a); }
double _ISINF_::operator() (const double a) const { return isinf(a); }

_ISNAN_ & _ISNAN_::singleton(void) { static _ISNAN_ a; return a; }
string _ISNAN_::str(void) const { return "isnan"; }
float _ISNAN_::operator() (const float a) const { return isnan(a); }
double _ISNAN_::operator() (const double a) const { return isnan(a); }

_ISNORMAL_ & _ISNORMAL_::singleton(void) { static _ISNORMAL_ a; return a; }
string _ISNORMAL_::str(void) const { return "isnormal"; }
float _ISNORMAL_::operator() (const float a) const { return isnormal(a); }
double _ISNORMAL_::operator() (const double a) const { return isnormal(a); }

DEFN_SIMPLE_FUN_FP1(LGAMMA, lgamma, lgammaf, lgamma)
DEFN_SIMPLE_FUN_FP1(LOG, log, logf, log)
DEFN_SIMPLE_FUN_FP1(LOG2, log2, log2f, log2)
DEFN_SIMPLE_FUN_FP1(LOG10, log10, log10f, log10)
DEFN_SIMPLE_FUN_FP1(LOG1P, log1p, log1pf, log1p)
DEFN_SIMPLE_FUN_FP1(LOGB, logb, logbf, logb)

_NAN_ & _NAN_::singleton(void) { static _NAN_ a; return a; }
string _NAN_::str(void) const { return "nan"; }
float _NAN_::operator() (const float a) const { return nan(""); }
double _NAN_::operator() (const double a) const { return nan(""); }

_NOT_ & _NOT_::singleton(void) { static _NOT_ a; return a; }
string _NOT_::str(void) const { return "!"; }
int32_t _NOT_::operator() (const int32_t a) const { return ! a; }
uint32_t _NOT_::operator() (const uint32_t a) const { return ! a; }

_RADIANS_ & _RADIANS_::singleton(void) { static _RADIANS_ a; return a; }
string _RADIANS_::str(void) const { return "radians"; }
float _RADIANS_::operator() (const float a) const { return a * M_PI / 180; }
double _RADIANS_::operator() (const double a) const { return a * M_PI / 180; }

DEFN_SIMPLE_FUN_FP1(RINT, rint, rintf, rint)
DEFN_SIMPLE_FUN_FP1(ROUND, round, roundf, round)

_RSQRT_ & _RSQRT_::singleton(void) { static _RSQRT_ a; return a; }
string _RSQRT_::str(void) const { return "rqrt"; }
float _RSQRT_::operator() (const float a) const { return 1.f / sqrtf(a); }
double _RSQRT_::operator() (const double a) const { return 1.f / sqrt(a); }

_SIGN_ & _SIGN_::singleton(void) { static _SIGN_ a; return a; }
string _SIGN_::str(void) const { return "sign"; }
float _SIGN_::operator() (const float a) const
{
    if (a > 0.0) return 1.0;
    else if (a < 0.0) return -1.0;
    else return a; // a must be -0.0 or +0.0
}
double _SIGN_::operator() (const double a) const
{
    if (a > 0.0) return 1.0;
    else if (a < 0.0) return -1.0;
    else return a; // a must be -0.0 or +0.0
}

DEFN_SIMPLE_FUN_FP1(SIGNBIT, signbit, signbit, signbit)
DEFN_SIMPLE_FUN_FP1(SIN, sin, sinf, sin)
DEFN_SIMPLE_FUN_FP1(SINH, sinh, sinhf, sinh)

_SINPI_ & _SINPI_::singleton(void) { static _SINPI_ a; return a; }
string _SINPI_::str(void) const { return "sinpi"; }
float _SINPI_::operator() (const float a) const { return sinf(M_PI * a); }
double _SINPI_::operator() (const double a) const { return sin(M_PI * a); }

DEFN_SIMPLE_FUN_FP1(SQRT, sqrt, sqrtf, sqrt)
DEFN_SIMPLE_FUN_FP1(TAN, tan, tanf, tan)
DEFN_SIMPLE_FUN_FP1(TANH, tanh, tanhf, tanh)

_TANPI_ & _TANPI_::singleton(void) { static _TANPI_ a; return a; }
string _TANPI_::str(void) const { return "tanpi"; }
float _TANPI_::operator() (const float a) const { return tanf(M_PI * a); }
double _TANPI_::operator() (const double a) const { return tan(M_PI * a); }

DEFN_SIMPLE_FUN_FP1(TGAMMA, tgamma, tgammaf, tgamma)
DEFN_SIMPLE_FUN_FP1(TRUNC, trunc, truncf, trunc)

_UNARYMINUS_ & _UNARYMINUS_::singleton(void)
{
    static _UNARYMINUS_ a;
    return a;
}
string _UNARYMINUS_::str(void) const { return "-"; }
float _UNARYMINUS_::operator() (const float a) const { return (- a); }
double _UNARYMINUS_::operator() (const double a) const { return (- a); }
int32_t _UNARYMINUS_::operator() (const int32_t a) const { return (- a); }

////////////////////////////////////////
// binary operators and functions

SimpleFun2::~SimpleFun2(void) { }

bool SimpleFun2::infix(void) const
{
    return false;
}

uint32_t SimpleFun2::operator() (const uint32_t,
                                 const uint32_t) const
{
    return 0;
}

int32_t SimpleFun2::operator() (const int32_t,
                                const int32_t) const
{
    return 0;
}

float SimpleFun2::operator() (const float,
                              const float) const
{
    return 0;
}

double SimpleFun2::operator() (const double,
                               const double) const
{
    return 0;
}

int32_t SimpleFun2::operator() (const uint32_t a, const int32_t b) const {
    return (*this)(static_cast<int32_t>(a), b); }
float SimpleFun2::operator() (const uint32_t a, const float b) const {
    return (*this)(static_cast<float>(a), b); }
double SimpleFun2::operator() (const uint32_t a, const double b) const {
    return (*this)(static_cast<double>(a), b); }
int32_t SimpleFun2::operator() (const int32_t a, const uint32_t b) const {
    return (*this)(a, static_cast<int32_t>(b)); }
float SimpleFun2::operator() (const int32_t a, const float b) const {
    return (*this)(static_cast<float>(a), b); }
double SimpleFun2::operator() (const int32_t a, const double b) const {
    return (*this)(static_cast<double>(a), b); }
float SimpleFun2::operator() (const float a, const uint32_t b) const {
    return (*this)(a, static_cast<float>(b)); }
float SimpleFun2::operator() (const float a, const int32_t b) const {
    return (*this)(a, static_cast<float>(b)); }
double SimpleFun2::operator() (const float a, const double b) const {
    return (*this)(static_cast<double>(a), b); }
double SimpleFun2::operator() (const double a, const uint32_t b) const {
    return (*this)(a, static_cast<double>(b)); }
double SimpleFun2::operator() (const double a, const int32_t b) const {
    return (*this)(a, static_cast<double>(b)); }
double SimpleFun2::operator() (const double a, const float b) const {
    return (*this)(a, static_cast<double>(b)); }

#define DEFN_SIMPLE_FUN_FP2(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return FUN32 (a, b); } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return FUN64 (a, b); }

#define DEFN_SIMPLE_FUN_INFIX2(X, FUN) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
bool _ ## X ## _ ::infix(void) const { return true; } \
string _ ## X ## _ ::str(void) const { return #FUN; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; } \
uint32_t _ ## X ## _ ::operator() (const uint32_t a, const uint32_t b) const { \
    return a FUN b; } \
int32_t _ ## X ## _ ::operator() (const int32_t a, const int32_t b) const { \
    return a FUN b; }

#define DEFN_SIMPLE_FUN_PREFIX2(X, NAME, FUN) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; }

#define DEFN_SIMPLE_FUN_INTFIX2(X, FUN) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
bool _ ## X ## _ ::infix(void) const { return true; } \
string _ ## X ## _ ::str(void) const { return #FUN; } \
uint32_t _ ## X ## _ ::operator() (const uint32_t a, const uint32_t b) const { \
    return a FUN b; } \
int32_t _ ## X ## _ ::operator() (const int32_t a, const int32_t b) const { \
    return a FUN b; }

_ABS_DIFF_ & _ABS_DIFF_::singleton(void) { static _ABS_DIFF_ a; return a; }
string _ABS_DIFF_::str(void) const { return "abs_diff"; }
uint32_t _ABS_DIFF_::operator() (const uint32_t a, const uint32_t b) const
{
    if (a < b)
        return b - a;
    else
        return a - b;
}
int32_t _ABS_DIFF_::operator() (const int32_t a, const int32_t b) const
{
    if (a < b)
        return b - a;
    else
        return a - b;
}

DEFN_SIMPLE_FUN_INFIX2(ADD, +)

_ADD_SAT_ & _ADD_SAT_::singleton(void) { static _ADD_SAT_ a; return a; }
string _ADD_SAT_::str(void) const { return "add_sat"; }
uint32_t _ADD_SAT_::operator() (const uint32_t a, const uint32_t b) const
{
    const uint64_t c = static_cast<uint64_t>(a) + static_cast<uint64_t>(b);
    return (c <= UINT32_MAX) ? c : UINT32_MAX;
}
int32_t _ADD_SAT_::operator() (const int32_t a, const int32_t b) const
{
    const int64_t c = static_cast<int64_t>(a) + static_cast<int64_t>(b);
    return (c <= INT32_MAX) ? c : INT32_MAX;
}

DEFN_SIMPLE_FUN_FP2(ATAN2, atan2, atan2f, atan2)

_ATAN2PI_ & _ATAN2PI_::singleton(void) { static _ATAN2PI_ a; return a; }
string _ATAN2PI_::str(void) const { return "atan2pi"; }
float _ATAN2PI_::operator() (const float a, const float b) const
{
    return atan2f(a, b) / M_PI;
}
double _ATAN2PI_::operator() (const double a, const double b) const
{
    return atan2(a, b) / M_PI;
}

DEFN_SIMPLE_FUN_INTFIX2(BITWISEAND, &)
DEFN_SIMPLE_FUN_INTFIX2(BITWISEOR, |)
DEFN_SIMPLE_FUN_INTFIX2(BITWISEXOR, ^)
DEFN_SIMPLE_FUN_FP2(COPYSIGN, copysign, copysignf, copysign)
DEFN_SIMPLE_FUN_INFIX2(DIVIDE, /)
DEFN_SIMPLE_FUN_INFIX2(EQUAL, ==)
DEFN_SIMPLE_FUN_FP2(FDIM, fdim, fdimf, fdim)
DEFN_SIMPLE_FUN_FP2(FMAX, fmax, fmaxf, fmax)
DEFN_SIMPLE_FUN_FP2(FMIN, fmin, fminf, fmin)
DEFN_SIMPLE_FUN_FP2(FMOD, fmod, fmodf, fmod)
DEFN_SIMPLE_FUN_INFIX2(GREATEREQUAL, >=)
DEFN_SIMPLE_FUN_INFIX2(GREATERTHAN, >)

_HADD_ & _HADD_::singleton(void) { static _HADD_ a; return a; }
string _HADD_::str(void) const { return "hadd"; }
uint32_t _HADD_::operator() (const uint32_t a, const uint32_t b) const
{
    const uint64_t c = static_cast<uint64_t>(a) + static_cast<uint64_t>(b);
    const uint32_t d = (c <= UINT32_MAX) ? c : UINT32_MAX;
    return d >> 1;
}
int32_t _HADD_::operator() (const int32_t a, const int32_t b) const
{
    const int64_t c = static_cast<int64_t>(a) + static_cast<int64_t>(b);
    const int32_t d = (c <= INT32_MAX) ? c : INT32_MAX;
    return d >> 1;
}

DEFN_SIMPLE_FUN_FP2(HYPOT, hypot, hypotf, hypot)
DEFN_SIMPLE_FUN_PREFIX2(ISEQUAL, isequal, ==)

_ISORDERED_ & _ISORDERED_::singleton(void) { static _ISORDERED_ a; return a; }
string _ISORDERED_::str(void) const { return "isordered"; }
float _ISORDERED_::operator() (const float a, const float b) const
{
    return a == a && b == b;
}
double _ISORDERED_::operator() (const double a, const double b) const
{
    return a == a && b == b;
}

_ISUNORDERED_ & _ISUNORDERED_::singleton(void)
{
    static _ISUNORDERED_ a;
    return a;
}
string _ISUNORDERED_::str(void) const { return "isunordered"; }
float _ISUNORDERED_::operator() (const float a, const float b) const
{
    return isnan(a) || isnan(b);
}
double _ISUNORDERED_::operator() (const double a, const double b) const
{
    return isnan(a) || isnan(b);
}

DEFN_SIMPLE_FUN_PREFIX2(ISNOTEQUAL, isnotequal, !=)
DEFN_SIMPLE_FUN_PREFIX2(ISGREATER, isgreater, >)
DEFN_SIMPLE_FUN_PREFIX2(ISGREATEREQUAL, isgreaterequal, >=)
DEFN_SIMPLE_FUN_PREFIX2(ISLESS, isless, <)
DEFN_SIMPLE_FUN_PREFIX2(ISLESSEQUAL, islessequal, <=)

_ISLESSGREATER_ & _ISLESSGREATER_::singleton(void)
{
    static _ISLESSGREATER_ a;
    return a;
}
string _ISLESSGREATER_::str(void) const { return "islessgreater"; }
float _ISLESSGREATER_::operator() (const float a, const float b) const
{
    return (a < b) || (a > b);
}
double _ISLESSGREATER_::operator() (const double a, const double b) const
{
    return (a < b) || (a > b);
}

DEFN_SIMPLE_FUN_FP2(LDEXP, ldexp, ldexpf, ldexp)
DEFN_SIMPLE_FUN_INFIX2(LESSEQUAL, <=)
DEFN_SIMPLE_FUN_INFIX2(LESSTHAN, <)

DEFN_SIMPLE_FUN_INTFIX2(LOGICALAND, &&)
DEFN_SIMPLE_FUN_INTFIX2(LOGICALOR, ||)

_MAX_ & _MAX_::singleton(void) { static _MAX_ a; return a; }
string _MAX_::str(void) const { return "max"; }
bool _MAX_::infix(void) const { return false; }
float _MAX_::operator() (const float a, const float b) const
{
    return a < b ? b : a;
}
double _MAX_::operator() (const double a, const double b) const
{
    return a < b ? b : a;
}
uint32_t _MAX_::operator() (const uint32_t a, const uint32_t b) const
{
    return a < b ? b : a;
}
int32_t _MAX_::operator() (const int32_t a, const int32_t b) const
{
    return a < b ? b : a;
}

_MAXMAG_ & _MAXMAG_::singleton(void) { static _MAXMAG_ a; return a; }
string _MAXMAG_::str(void) const { return "maxmag"; }
float _MAXMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) > fabsf(b)) return a;
    else if (fabsf(a) < fabsf(b)) return b;
    else fmaxf(a, b);
}
double _MAXMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) > fabs(b)) return a;
    else if (fabs(a) < fabs(b)) return b;
    else fmax(a, b);
}

_MIN_ & _MIN_::singleton(void) { static _MIN_ a; return a; }
string _MIN_::str(void) const { return "min"; }
bool _MIN_::infix(void) const { return false; }
float _MIN_::operator() (const float a, const float b) const
{
    return a < b ? a : b;
}
double _MIN_::operator() (const double a, const double b) const
{
    return a < b ? a : b;
}
uint32_t _MIN_::operator() (const uint32_t a, const uint32_t b) const
{
    return a < b ? a : b;
}
int32_t _MIN_::operator() (const int32_t a, const int32_t b) const
{
    return a < b ? a : b;
}

_MINMAG_ & _MINMAG_::singleton(void) { static _MINMAG_ a; return a; }
string _MINMAG_::str(void) const { return "minmag"; }
float _MINMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) < fabsf(b)) return a;
    else if (fabsf(a) > fabsf(b)) return b;
    else fminf(a, b);
}
double _MINMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) < fabs(b)) return a;
    else if (fabs(a) > fabs(b)) return b;
    else fmin(a, b);
}

DEFN_SIMPLE_FUN_INTFIX2(MODULO, %)
DEFN_SIMPLE_FUN_INFIX2(MULTIPLY, *)

_MUL24_ & _MUL24_::singleton(void) { static _MUL24_ a; return a; }
string _MUL24_::str(void) const { return "mul24"; }
uint32_t _MUL24_::operator() (const uint32_t a, const uint32_t b) const
{
    const uint32_t a2 = a & 0x00ffffff;
    const uint32_t b2 = b & 0x00ffffff;
    return a2 * b2;
}
int32_t _MUL24_::operator() (const int32_t a, const int32_t b) const
{
    const int32_t a2 = a & 0x00ffffff;
    const int32_t b2 = b & 0x00ffffff;
    return a2 * b2;
}

_MUL_HI_ & _MUL_HI_::singleton(void) { static _MUL_HI_ a; return a; }
string _MUL_HI_::str(void) const { return "mul_hi"; }
uint32_t _MUL_HI_::operator() (const uint32_t a, const uint32_t b) const
{
    const uint64_t c = static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
    const uint32_t d = c >> 32;
    return d;
}
int32_t _MUL_HI_::operator() (const int32_t a, const int32_t b) const
{
    const int64_t c = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    const int32_t d = c >> 32;
    return d;
}

DEFN_SIMPLE_FUN_INFIX2(NOTEQUAL, !=)
DEFN_SIMPLE_FUN_FP2(NEXTAFTER, nextafter, nextafterf, nextafter)

DEFN_SIMPLE_FUN_FP2(POW, pow, powf, pow)

_POWN_ & _POWN_::singleton(void) { static _POWN_ a; return a; }
string _POWN_::str(void) const { return "pown"; }
float _POWN_::operator() (const float a, const float b) const
{
    return powf(a, static_cast<int>(b));
}
double _POWN_::operator() (const double a, const double b) const
{
    return pow(a, static_cast<int>(b));
}

DEFN_SIMPLE_FUN_FP2(POWR, pow, powf, pow)
DEFN_SIMPLE_FUN_FP2(REMAINDER, remainder, remainderf, remainder)

_RHADD_ & _RHADD_::singleton(void) { static _RHADD_ a; return a; }
string _RHADD_::str(void) const { return "rhadd"; }
uint32_t _RHADD_::operator() (const uint32_t a, const uint32_t b) const
{
    const uint64_t c = static_cast<uint64_t>(a) + static_cast<uint64_t>(b) + 1;
    const uint32_t d = (c <= UINT32_MAX) ? c : UINT32_MAX;
    return d >> 1;
}
int32_t _RHADD_::operator() (const int32_t a, const int32_t b) const
{
    const int64_t c = static_cast<int64_t>(a) + static_cast<int64_t>(b) + 1;
    const int32_t d = (c <= INT32_MAX) ? c : INT32_MAX;
    return d >> 1;
}

_ROOTN_ & _ROOTN_::singleton(void) { static _ROOTN_ a; return a; }
string _ROOTN_::str(void) const { return "rootn"; }
float _ROOTN_::operator() (const float a, const float b) const
{
    return powf(a, 1.f / b);
}
double _ROOTN_::operator() (const double a, const double b) const
{
    return pow(a, 1.f / b);
}

_ROTATE_ & _ROTATE_::singleton(void) { static _ROTATE_ a; return a; }
string _ROTATE_::str(void) const { return "rotate"; }
uint32_t _ROTATE_::operator() (const uint32_t a, const uint32_t b) const
{
    return (a << b) | (a >> (32 - b));
}
int32_t _ROTATE_::operator() (const int32_t a, const int32_t b) const
{
    return (a << b) | (a >> (32 - b));
}

DEFN_SIMPLE_FUN_INTFIX2(SHIFTLEFT, <<)
DEFN_SIMPLE_FUN_INTFIX2(SHIFTRIGHT, >>)

_STEP_ & _STEP_::singleton(void) { static _STEP_ a; return a; }
string _STEP_::str(void) const { return "step"; }
float _STEP_::operator() (const float a, const float b) const
{
    return b < a ? 0.0 : 1.0;
}
double _STEP_::operator() (const double a, const double b) const
{
    return b < a ? 0.0 : 1.0;
}

DEFN_SIMPLE_FUN_INFIX2(SUBTRACT, -)

_SUB_SAT_ & _SUB_SAT_::singleton(void) { static _SUB_SAT_ a; return a; }
string _SUB_SAT_::str(void) const { return "sub_sat"; }
uint32_t _SUB_SAT_::operator() (const uint32_t a, const uint32_t b) const
{
    if (a < b)
        return 0;
    else
        return b - a;
}
int32_t _SUB_SAT_::operator() (const int32_t a, const int32_t b) const
{
    const int64_t c = static_cast<int64_t>(a) - static_cast<int64_t>(b);
    if (c < INT32_MIN)
        return INT32_MIN;
    else if (c > INT32_MAX)
        return INT32_MAX;
    else
        return c;
}

////////////////////////////////////////
// functions with three arguments

SimpleFun3::~SimpleFun3(void) { }

uint32_t SimpleFun3::operator() (const uint32_t,
                                 const uint32_t,
                                 const uint32_t) const
{
    return 0;
}

int32_t SimpleFun3::operator() (const int32_t,
                                const int32_t,
                                const int32_t) const
{
    return 0;
}

float SimpleFun3::operator() (const float,
                              const float,
                              const float) const
{
    return 0;
}

double SimpleFun3::operator() (const double,
                               const double,
                               const double) const
{
    return 0;
}

int32_t SimpleFun3::operator() (const uint32_t a,
                                const uint32_t b,
                                const int32_t c) const
{
    return (*this)(static_cast<int32_t>(a),
                   static_cast<int32_t>(b),
                   c);
}

float SimpleFun3::operator() (const uint32_t a,
                              const uint32_t b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const uint32_t a,
                               const uint32_t b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

int32_t SimpleFun3::operator() (const uint32_t a,
                                const int32_t b,
                                const uint32_t c) const
{
    return (*this)(static_cast<int32_t>(a),
                   b,
                   static_cast<int32_t>(c));
}

int32_t SimpleFun3::operator() (const uint32_t a,
                                const int32_t b,
                                const int32_t c) const
{
    return (*this)(static_cast<int32_t>(a),
                   b,
                   c);
}

float SimpleFun3::operator() (const uint32_t a,
                              const int32_t b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const uint32_t a,
                               const int32_t b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

float SimpleFun3::operator() (const uint32_t a,
                              const float b,
                              const uint32_t c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const uint32_t a,
                              const float b,
                              const int32_t c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const uint32_t a,
                              const float b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   c);
}

double SimpleFun3::operator() (const uint32_t a,
                               const float b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const uint32_t a,
                               const double b,
                               const uint32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const uint32_t a,
                               const double b,
                               const int32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const uint32_t a,
                               const double b,
                               const float c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const uint32_t a,
                               const double b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   c);
}

int32_t SimpleFun3::operator() (const int32_t a,
                                const uint32_t b,
                                const uint32_t c) const
{
    return (*this)(a,
                   static_cast<int32_t>(b),
                   static_cast<int32_t>(c));
}

int32_t SimpleFun3::operator() (const int32_t a,
                                const uint32_t b,
                                const int32_t c) const
{
    return (*this)(a,
                   static_cast<int32_t>(b),
                   c);
}

float SimpleFun3::operator() (const int32_t a,
                              const uint32_t b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const int32_t a,
                               const uint32_t b,
                               const double c) const
{
    return (*this)(static_cast<float>(a),
                   static_cast<float>(b),
                   c);
}


int32_t SimpleFun3::operator() (const int32_t a,
                                const int32_t b,
                                const uint32_t c) const
{
    return (*this)(a,
                   b,
                   static_cast<int32_t>(c));
}

float SimpleFun3::operator() (const int32_t a,
                              const int32_t b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const int32_t a,
                               const int32_t b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

float SimpleFun3::operator() (const int32_t a,
                              const float b,
                              const uint32_t c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const int32_t a,
                              const float b,
                              const int32_t c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const int32_t a,
                              const float b,
                              const float c) const
{
    return (*this)(static_cast<float>(a),
                   b,
                   c);
}

double SimpleFun3::operator() (const int32_t a,
                               const float b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const int32_t a,
                               const double b,
                               const uint32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const int32_t a,
                               const double b,
                               const int32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const int32_t a,
                               const double b,
                               const float c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const int32_t a,
                               const double b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   c);
}

float SimpleFun3::operator() (const float a,
                              const uint32_t b,
                              const uint32_t c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const float a,
                              const uint32_t b,
                              const int32_t c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const float a,
                              const uint32_t b,
                              const float c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const float a,
                               const uint32_t b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

float SimpleFun3::operator() (const float a,
                              const int32_t b,
                              const uint32_t c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const float a,
                              const int32_t b,
                              const int32_t c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const float a,
                              const int32_t b,
                              const float c) const
{
    return (*this)(a,
                   static_cast<float>(b),
                   c);
}

double SimpleFun3::operator() (const float a,
                               const int32_t b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

float SimpleFun3::operator() (const float a,
                              const float b,
                              const uint32_t c) const
{
    return (*this)(a,
                   b,
                   static_cast<float>(c));
}

float SimpleFun3::operator() (const float a,
                              const float b,
                              const int32_t c) const
{
    return (*this)(a,
                   b,
                   static_cast<float>(c));
}

double SimpleFun3::operator() (const float a,
                               const float b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const float a,
                               const double b,
                               const uint32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const float a,
                               const double b,
                               const int32_t c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const float a,
                               const double b,
                               const float c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const float a,
                               const double b,
                               const double c) const
{
    return (*this)(static_cast<double>(a),
                   b,
                   c);
}

double SimpleFun3::operator() (const double a,
                               const uint32_t b,
                               const uint32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const uint32_t b,
                               const int32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const uint32_t b,
                               const float c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const uint32_t b,
                               const double c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const double a,
                               const int32_t b,
                               const uint32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const int32_t b,
                               const int32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const int32_t b,
                               const float c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const int32_t b,
                               const double c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const double a,
                               const float b,
                               const uint32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const float b,
                               const int32_t c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const float b,
                               const float c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const float b,
                               const double c) const
{
    return (*this)(a,
                   static_cast<double>(b),
                   c);
}

double SimpleFun3::operator() (const double a,
                               const double b,
                               const uint32_t c) const
{
    return (*this)(a,
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const double b,
                               const int32_t c) const
{
    return (*this)(a,
                   b,
                   static_cast<double>(c));
}

double SimpleFun3::operator() (const double a,
                               const double b,
                               const float c) const
{
    return (*this)(a,
                   b,
                   static_cast<double>(c));
}

#define DEFN_SIMPLE_FUN_FP3(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, \
                                const float b, \
                                const float c) const { \
    return FUN32 (a, b, c); } \
double _ ## X ## _ ::operator() (const double a, \
                                 const double b, \
                                 const double c) const { \
    return FUN64 (a, b, c); }

_CLAMP_ & _CLAMP_::singleton(void) { static _CLAMP_ a; return a; }
string _CLAMP_::str(void) const { return "clamp"; }
uint32_t _CLAMP_::operator() (const uint32_t a,
                              const uint32_t b,
                              const uint32_t c) const
{
    const uint32_t m = (a < b ? b : a);
    return m < c ? m : c;
}
int32_t _CLAMP_::operator() (const int32_t a,
                             const int32_t b,
                             const int32_t c) const
{
    const int32_t m = (a < b ? b : a);
    return m < c ? m : c;
}
float _CLAMP_::operator() (const float a, const float b, const float c) const
{
    const float m = (a < b ? b : a);
    return m < c ? m : c;
}
double _CLAMP_::operator() (const double a, const double b, const double c)
    const
{
    const double m = (a < b ? b : a);
    return m < c ? m : c;
}

DEFN_SIMPLE_FUN_FP3(FMA, fma, fmaf, fma)

_MAD_ & _MAD_::singleton(void) { static _MAD_ a; return a; }
string _MAD_::str(void) const { return "mad"; }
float _MAD_::operator() (const float a, const float b, const float c) const
{
    return a * b + c;
}
double _MAD_::operator() (const double a, const double b, const double c) const
{
    return a * b + c;
}

_MAD24_ & _MAD24_::singleton(void) { static _MAD24_ a; return a; }
string _MAD24_::str(void) const { return "mad24"; }
uint32_t _MAD24_::operator() (const uint32_t a,
                              const uint32_t b,
                              const uint32_t c) const
{
    return _MUL24_::singleton()(a, b) + c;
}
int32_t _MAD24_::operator() (const int32_t a,
                             const int32_t b,
                             const int32_t c) const
{
    return _MUL24_::singleton()(a, b) + c;
}

_MAD_HI_ & _MAD_HI_::singleton(void) { static _MAD_HI_ a; return a; }
string _MAD_HI_::str(void) const { return "mad_hi"; }
uint32_t _MAD_HI_::operator() (const uint32_t a,
                               const uint32_t b,
                               const uint32_t c) const
{
    return _MUL_HI_::singleton()(a, b) + c;
}
int32_t _MAD_HI_::operator() (const int32_t a,
                              const int32_t b,
                              const int32_t c) const
{
    return _MUL_HI_::singleton()(a, b) + c;
}

_MAD_SAT_ & _MAD_SAT_::singleton(void) { static _MAD_SAT_ a; return a; }
string _MAD_SAT_::str(void) const { return "mad_sat"; }
uint32_t _MAD_SAT_::operator() (const uint32_t a,
                                const uint32_t b,
                                const uint32_t c) const
{
    const uint64_t d = static_cast<uint64_t>(a) * static_cast<uint64_t>(b) + c;
    if (d > UINT32_MAX)
        return UINT32_MAX;
    else
        return d;
}
int32_t _MAD_SAT_::operator() (const int32_t a,
                               const int32_t b,
                               const int32_t c) const
{
    const int64_t d = static_cast<int64_t>(a) * static_cast<int64_t>(b) + c;
    if (d < INT32_MIN)
        return INT32_MIN;
    else if (d > INT32_MAX)
        return INT32_MAX;
    else
        return d;
}

_MIX_ & _MIX_::singleton(void) { static _MIX_ a; return a; }
string _MIX_::str(void) const { return "mix"; }
float _MIX_::operator() (const float a, const float b, const float c) const
{
    return a + (b - a) * c;
}
double _MIX_::operator() (const double a, const double b, const double c) const
{
    return a + (b - a) * c;
}

_SMOOTHSTEP_ & _SMOOTHSTEP_::singleton(void)
{
    static _SMOOTHSTEP_ a;
    return a;
}
string _SMOOTHSTEP_::str(void) const { return "smoothstep"; }
float _SMOOTHSTEP_::operator() (const float a, const float b, const float c) const
{
    // smoothstep(edge0, edge1, x)
    // t = clamp ((x – edge0) / (edge1 – edge0), 0, 1);
    // return t * t * (3 – 2 * t);

    const float d  = (c - a) / (b - a);
    const float d0 = (d < 0.0 ? 0.0 : d);
    const float t  = (d0 < 1.0 ? d0 : 1.0);
    return t * t * (3 - 2 * t);
}
double _SMOOTHSTEP_::operator() (const double a, const double b, const double c) const
{
    // smoothstep(edge0, edge1, x)
    // t = clamp ((x – edge0) / (edge1 – edge0), 0, 1);
    // return t * t * (3 – 2 * t);

    const double d  = (c - a) / (b - a);
    const double d0 = (d < 0.0 ? 0.0 : d);
    const double t  = (d0 < 1.0 ? d0 : 1.0);
    return t * t * (3 - 2 * t);
}

}; // namespace chai_internal
