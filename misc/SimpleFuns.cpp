// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>

#include "SimpleFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// unary functions

SimpleFun1::~SimpleFun1(void) { }

#define DEFN_SIMPLE_FUN1(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a) const { return FUN32 (a); } \
double _ ## X ## _ ::operator() (const double a) const { return FUN64 (a); }

DEFN_SIMPLE_FUN1(ACOS, acos, acosf, acos)
DEFN_SIMPLE_FUN1(ACOSH, acosh, acoshf, acosh)

_ACOSPI_ & _ACOSPI_::singleton(void) { static _ACOSPI_ a; return a; }
string _ACOSPI_::str(void) const { return "acospi"; }
float _ACOSPI_::operator() (const float a) const { return acosf(a) / M_PI; }
double _ACOSPI_::operator() (const double a) const { return acos(a) / M_PI; }

DEFN_SIMPLE_FUN1(ASIN, asin, asinf, asin)
DEFN_SIMPLE_FUN1(ASINH, asinh, asinhf, asinh)

_ASINPI_ & _ASINPI_::singleton(void) { static _ASINPI_ a; return a; }
string _ASINPI_::str(void) const { return "asinpi"; }
float _ASINPI_::operator() (const float a) const { return asinf(a) / M_PI; }
double _ASINPI_::operator() (const double a) const { return asin(a) / M_PI; }

DEFN_SIMPLE_FUN1(ATAN, atan, atanf, atan)
DEFN_SIMPLE_FUN1(ATANH, atanh, atanhf, atanh)

_ATANPI_ & _ATANPI_::singleton(void) { static _ATANPI_ a; return a; }
string _ATANPI_::str(void) const { return "atanpi"; }
float _ATANPI_::operator() (const float a) const { return atanf(a) / M_PI; }
double _ATANPI_::operator() (const double a) const { return atan(a) / M_PI; }

DEFN_SIMPLE_FUN1(CBRT, cbrt, cbrtf, cbrt)
DEFN_SIMPLE_FUN1(CEIL, ceil, ceilf, ceil)
DEFN_SIMPLE_FUN1(COS, cos, cosf, cos)
DEFN_SIMPLE_FUN1(COSH, cosh, coshf, cosh)

_COSPI_ & _COSPI_::singleton(void) { static _COSPI_ a; return a; }
string _COSPI_::str(void) const { return "cospi"; }
float _COSPI_::operator() (const float a) const { return cosf(M_PI * a); }
double _COSPI_::operator() (const double a) const { return cos(M_PI * a); }

_DEGREES_ & _DEGREES_::singleton(void) { static _DEGREES_ a; return a; }
string _DEGREES_::str(void) const { return "degrees"; }
float _DEGREES_::operator() (const float a) const { return M_1_PI * 180 * a; }
double _DEGREES_::operator() (const double a) const { return M_1_PI * 180 * a; }

DEFN_SIMPLE_FUN1(ERFC, erfc, erfcf, erfc)
DEFN_SIMPLE_FUN1(ERF, erf, erff, erf)
DEFN_SIMPLE_FUN1(EXP, exp, expf, exp)
DEFN_SIMPLE_FUN1(EXP2, exp2, exp2f, exp2)
DEFN_SIMPLE_FUN1(EXP10, exp10, exp10f, exp10)
DEFN_SIMPLE_FUN1(EXPM1, expm1, expm1f, expm1)
DEFN_SIMPLE_FUN1(FABS, fabs, fabsf, fabs)
DEFN_SIMPLE_FUN1(FLOOR, floor, floorf, floor)
DEFN_SIMPLE_FUN1(ILOGB, ilogb, ilogbf, ilogb)

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

DEFN_SIMPLE_FUN1(LGAMMA, lgamma, lgammaf, lgamma)
DEFN_SIMPLE_FUN1(LOG, log, logf, log)
DEFN_SIMPLE_FUN1(LOG2, log2, log2f, log2)
DEFN_SIMPLE_FUN1(LOG10, log10, log10f, log10)
DEFN_SIMPLE_FUN1(LOG1P, log1p, log1pf, log1p)
DEFN_SIMPLE_FUN1(LOGB, logb, logbf, logb)

_NAN_ & _NAN_::singleton(void) { static _NAN_ a; return a; }
string _NAN_::str(void) const { return "nan"; }
float _NAN_::operator() (const float a) const { return nan(""); }
double _NAN_::operator() (const double a) const { return nan(""); }

DEFN_SIMPLE_FUN1(NEGATE, -, -, -)

_RADIANS_ & _RADIANS_::singleton(void) { static _RADIANS_ a; return a; }
string _RADIANS_::str(void) const { return "radians"; }
float _RADIANS_::operator() (const float a) const { return a * M_PI / 180; }
double _RADIANS_::operator() (const double a) const { return a * M_PI / 180; }

DEFN_SIMPLE_FUN1(RINT, rint, rintf, rint)
DEFN_SIMPLE_FUN1(ROUND, round, roundf, round)

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

DEFN_SIMPLE_FUN1(SIGNBIT, signbit, signbit, signbit)
DEFN_SIMPLE_FUN1(SIN, sin, sinf, sin)
DEFN_SIMPLE_FUN1(SINH, sinh, sinhf, sinh)

_SINPI_ & _SINPI_::singleton(void) { static _SINPI_ a; return a; }
string _SINPI_::str(void) const { return "sinpi"; }
float _SINPI_::operator() (const float a) const { return sinf(M_PI * a); }
double _SINPI_::operator() (const double a) const { return sin(M_PI * a); }

DEFN_SIMPLE_FUN1(SQRT, sqrt, sqrtf, sqrt)
DEFN_SIMPLE_FUN1(TAN, tan, tanf, tan)
DEFN_SIMPLE_FUN1(TANH, tanh, tanhf, tanh)

_TANPI_ & _TANPI_::singleton(void) { static _TANPI_ a; return a; }
string _TANPI_::str(void) const { return "tanpi"; }
float _TANPI_::operator() (const float a) const { return tanf(M_PI * a); }
double _TANPI_::operator() (const double a) const { return tan(M_PI * a); }

DEFN_SIMPLE_FUN1(TGAMMA, tgamma, tgammaf, tgamma)
DEFN_SIMPLE_FUN1(TRUNC, trunc, truncf, trunc)

////////////////////////////////////////
// binary operators and functions

SimpleFun2::~SimpleFun2(void) { }

#define DEFN_SIMPLE_BINOP(X, FUN) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
bool _ ## X ## _ ::infix(void) const { return true; } \
string _ ## X ## _ ::str(void) const { return #FUN; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const float a, const double b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; }

DEFN_SIMPLE_BINOP(ADD, +)
DEFN_SIMPLE_BINOP(AND,  &&)
DEFN_SIMPLE_BINOP(DIV, /)
DEFN_SIMPLE_BINOP(EQ, ==)
DEFN_SIMPLE_BINOP(GE, >=)
DEFN_SIMPLE_BINOP(GT, >)
DEFN_SIMPLE_BINOP(LE, <=)
DEFN_SIMPLE_BINOP(LT, <)
DEFN_SIMPLE_BINOP(MUL, *)
DEFN_SIMPLE_BINOP(NE, !=)
DEFN_SIMPLE_BINOP(OR, ||)
DEFN_SIMPLE_BINOP(SUB, -)

_ISORDERED_ & _ISORDERED_::singleton(void) { static _ISORDERED_ a; return a; }
bool _ISORDERED_::infix(void) const { return false; }
string _ISORDERED_::str(void) const { return "isordered"; }
float _ISORDERED_::operator() (const float a, const float b) const
{
    return a == a && b == b;
}
double _ISORDERED_::operator() (const float a, const double b) const
{
    return a == a && b == b;
}
double _ISORDERED_::operator() (const double a, const float b) const
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
bool _ISUNORDERED_::infix(void) const { return false; }
string _ISUNORDERED_::str(void) const { return "isunordered"; }
float _ISUNORDERED_::operator() (const float a, const float b) const
{
    return isnan(a) || isnan(b);
}
double _ISUNORDERED_::operator() (const float a, const double b) const
{
    return isnan(a) || isnan(b);
}
double _ISUNORDERED_::operator() (const double a, const float b) const
{
    return isnan(a) || isnan(b);
}
double _ISUNORDERED_::operator() (const double a, const double b) const
{
    return isnan(a) || isnan(b);
}

_MAX_ & _MAX_::singleton(void) { static _MAX_ a; return a; }
bool _MAX_::infix(void) const { return false; }
string _MAX_::str(void) const { return "max"; }
float _MAX_::operator() (const float a, const float b) const
{
    return a < b ? b : a;
}
double _MAX_::operator() (const float a, const double b) const
{
    return a < b ? b : a;
}
double _MAX_::operator() (const double a, const float b) const
{
    return a < b ? b : a;
}
double _MAX_::operator() (const double a, const double b) const
{
    return a < b ? b : a;
}

_MIN_ & _MIN_::singleton(void) { static _MIN_ a; return a; }
bool _MIN_::infix(void) const { return false; }
string _MIN_::str(void) const { return "min"; }
float _MIN_::operator() (const float a, const float b) const
{
    return a < b ? a : b;
}
double _MIN_::operator() (const float a, const double b) const
{
    return a < b ? a : b;
}
double _MIN_::operator() (const double a, const float b) const
{
    return a < b ? a : b;
}
double _MIN_::operator() (const double a, const double b) const
{
    return a < b ? a : b;
}

_MAXMAG_ & _MAXMAG_::singleton(void) { static _MAXMAG_ a; return a; }
bool _MAXMAG_::infix(void) const { return false; }
string _MAXMAG_::str(void) const { return "maxmag"; }
float _MAXMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) > fabsf(b)) return a;
    else if (fabsf(a) < fabsf(b)) return b;
    else fmaxf(a, b);
}
double _MAXMAG_::operator() (const float a, const double b) const
{
    if (fabsf(a) > fabs(b)) return a;
    else if (fabsf(a) < fabs(b)) return b;
    else fmax(a, b);
}
double _MAXMAG_::operator() (const double a, const float b) const
{
    if (fabs(a) > fabsf(b)) return a;
    else if (fabs(a) < fabsf(b)) return b;
    else fmax(a, b);
}
double _MAXMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) > fabs(b)) return a;
    else if (fabs(a) < fabs(b)) return b;
    else fmax(a, b);
}

_MINMAG_ & _MINMAG_::singleton(void) { static _MINMAG_ a; return a; }
bool _MINMAG_::infix(void) const { return false; }
string _MINMAG_::str(void) const { return "minmag"; }
float _MINMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) < fabsf(b)) return a;
    else if (fabsf(a) > fabsf(b)) return b;
    else fminf(a, b);
}
double _MINMAG_::operator() (const float a, const double b) const
{
    if (fabsf(a) < fabs(b)) return a;
    else if (fabsf(a) > fabs(b)) return b;
    else fmin(a, b);
}
double _MINMAG_::operator() (const double a, const float b) const
{
    if (fabs(a) < fabsf(b)) return a;
    else if (fabs(a) > fabsf(b)) return b;
    else fmin(a, b);
}
double _MINMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) < fabs(b)) return a;
    else if (fabs(a) > fabs(b)) return b;
    else fmin(a, b);
}

#define DEFN_SIMPLE_FUN2(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
bool _ ## X ## _ ::infix(void) const { return false; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return FUN32 (a, b); } \
double _ ## X ## _ ::operator() (const float a, const double b) const { \
    return FUN64 (a, b); } \
double _ ## X ## _ ::operator() (const double a, const float b) const { \
    return FUN64 (a, b); } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return FUN64 (a, b); }

DEFN_SIMPLE_FUN2(NEXTAFTER, nextafter, nextafterf, nextafter)
DEFN_SIMPLE_FUN2(POW, pow, powf, pow)

_POWN_ & _POWN_::singleton(void) { static _POWN_ a; return a; }
bool _POWN_::infix(void) const { return false; }
string _POWN_::str(void) const { return "pown"; }
float _POWN_::operator() (const float a, const float b) const
{
    return powf(a, static_cast<int>(b));
}
double _POWN_::operator() (const float a, const double b) const
{
    return powf(a, static_cast<int>(b)); // note: powf return float
}
double _POWN_::operator() (const double a, const float b) const
{
    return pow(a, static_cast<int>(b));
}
double _POWN_::operator() (const double a, const double b) const
{
    return pow(a, static_cast<int>(b));
}

DEFN_SIMPLE_FUN2(POWR, pow, powf, pow)
DEFN_SIMPLE_FUN2(REMAINDER, remainder, remainderf, remainder)

_ROOTN_ & _ROOTN_::singleton(void) { static _ROOTN_ a; return a; }
bool _ROOTN_::infix(void) const { return false; }
string _ROOTN_::str(void) const { return "rootn"; }
float _ROOTN_::operator() (const float a, const float b) const
{
    return powf(a, 1.f / b);
}
double _ROOTN_::operator() (const float a, const double b) const
{
    return pow(a, 1.f / b);
}
double _ROOTN_::operator() (const double a, const float b) const
{
    return pow(a, 1.f / b);
}
double _ROOTN_::operator() (const double a, const double b) const
{
    return pow(a, 1.f / b);
}

_STEP_ & _STEP_::singleton(void) { static _STEP_ a; return a; }
bool _STEP_::infix(void) const { return false; }
string _STEP_::str(void) const { return "step"; }
float _STEP_::operator() (const float a, const float b) const
{
    return b < a ? 0.0 : 1.0;
}
double _STEP_::operator() (const float a, const double b) const
{
    return b < a ? 0.0 : 1.0;
}
double _STEP_::operator() (const double a, const float b) const
{
    return b < a ? 0.0 : 1.0;
}
double _STEP_::operator() (const double a, const double b) const
{
    return b < a ? 0.0 : 1.0;
}

DEFN_SIMPLE_FUN2(ATAN2, atan2, atan2f, atan2)

_ATAN2PI_ & _ATAN2PI_::singleton(void) { static _ATAN2PI_ a; return a; }
bool _ATAN2PI_::infix(void) const { return false; }
string _ATAN2PI_::str(void) const { return "atan2pi"; }
float _ATAN2PI_::operator() (const float a, const float b) const
{
    return atan2f(a, b) / M_PI;
}
double _ATAN2PI_::operator() (const float a, const double b) const
{
    return atan2(a, b) / M_PI;
}
double _ATAN2PI_::operator() (const double a, const float b) const
{
    return atan2(a, b) / M_PI;
}
double _ATAN2PI_::operator() (const double a, const double b) const
{
    return atan2(a, b) / M_PI;
}

DEFN_SIMPLE_FUN2(COPYSIGN, copysign, copysignf, copysign)
DEFN_SIMPLE_FUN2(FDIM, fdim, fdimf, fdim)
DEFN_SIMPLE_FUN2(FMAX, fmax, fmaxf, fmax)
DEFN_SIMPLE_FUN2(FMIN, fmin, fminf, fmin)
DEFN_SIMPLE_FUN2(FMOD, fmod, fmodf, fmod)
DEFN_SIMPLE_FUN2(HYPOT, hypot, hypotf, hypot)
DEFN_SIMPLE_FUN2(LDEXP, ldexp, ldexpf, ldexp)

#define DEFN_SIMPLE_RELATIONAL(X, NAME, FUN) \
_ ## X ## _ & _ ## X ## _ ::singleton(void) \
    { static _ ## X ## _ a; return a; } \
bool _ ## X ## _ ::infix(void) const { return false; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const float a, const double b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const float b) const { \
    return a FUN b; } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; }

DEFN_SIMPLE_RELATIONAL(ISEQUAL, isequal, ==)
DEFN_SIMPLE_RELATIONAL(ISNOTEQUAL, isnotequal, !=)
DEFN_SIMPLE_RELATIONAL(ISGREATER, isgreater, >)
DEFN_SIMPLE_RELATIONAL(ISGREATEREQUAL, isgreaterequal, >=)
DEFN_SIMPLE_RELATIONAL(ISLESS, isless, <)
DEFN_SIMPLE_RELATIONAL(ISLESSEQUAL, islessequal, <=)

_ISLESSGREATER_ & _ISLESSGREATER_::singleton(void)
{
    static _ISLESSGREATER_ a;
    return a;
}
bool _ISLESSGREATER_::infix(void) const { return false; }
string _ISLESSGREATER_::str(void) const { return "islessgreater"; }
float _ISLESSGREATER_::operator() (const float a, const float b) const
{
    return (a < b) || (a > b);
}
double _ISLESSGREATER_::operator() (const float a, const double b) const
{
    return (a < b) || (a > b);
}
double _ISLESSGREATER_::operator() (const double a, const float b) const
{
    return (a < b) || (a > b);
}
double _ISLESSGREATER_::operator() (const double a, const double b) const
{
    return (a < b) || (a > b);
}

////////////////////////////////////////
// functions with three arguments

SimpleFun3::~SimpleFun3(void) { }

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
                               const float c) const
{
    return (*this)(a,
                   b,
                   static_cast<double>(c));
}

#define DEFN_SIMPLE_FUN3(X, NAME, FUN32, FUN64) \
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

DEFN_SIMPLE_FUN3(FMA, fma, fmaf, fma)

_MAD_ & _MAD_::singleton(void) { static _MAD_ a; return a; }
string _MAD_::str(void) const { return "mad"; }
float _MAD_::operator() (const float a,
                         const float b,
                         const float c) const
{
    return a * b + c;
}
double _MAD_::operator() (const double a,
                          const double b,
                          const double c) const
{
    return a * b + c;
}

_CLAMP_ & _CLAMP_::singleton(void) { static _CLAMP_ a; return a; }
string _CLAMP_::str(void) const { return "clamp"; }
float _CLAMP_::operator() (const float a,
                           const float b,
                           const float c) const
{
    const float m = (a < b ? b : a);
    return m < c ? m : c;
}
double _CLAMP_::operator() (const double a,
                            const double b,
                            const double c) const
{
    const double m = (a < b ? b : a);
    return m < c ? m : c;
}

_MIX_ & _MIX_::singleton(void) { static _MIX_ a; return a; }
string _MIX_::str(void) const { return "mix"; }
float _MIX_::operator() (const float a,
                         const float b,
                         const float c) const
{
    return a + (b - a) * c;
}
double _MIX_::operator() (const double a,
                          const double b,
                          const double c) const
{
    return a + (b - a) * c;
}

_SMOOTHSTEP_ & _SMOOTHSTEP_::singleton(void)
{
    static _SMOOTHSTEP_ a;
    return a;
}
string _SMOOTHSTEP_::str(void) const { return "smoothstep"; }
float _SMOOTHSTEP_::operator() (const float a,
                                const float b,
                                const float c) const
{
    // smoothstep(edge0, edge1, x)
    // t = clamp ((x – edge0) / (edge1 – edge0), 0, 1);
    // return t * t * (3 – 2 * t);

    const float d  = (c - a) / (b - a);
    const float d0 = (d < 0.0 ? 0.0 : d);
    const float t  = (d0 < 1.0 ? d0 : 1.0);
    return t * t * (3 - 2 * t);
}
double _SMOOTHSTEP_::operator() (const double a,
                                 const double b,
                                 const double c) const
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
