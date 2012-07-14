// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <stdlib.h>

#include "SimpleFun2.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// binary operators and functions

SimpleFun2::~SimpleFun2(void) { }

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

int32_t SimpleFun2::operator() (const uint32_t a, const int32_t b) const
{
    return (*this)(static_cast<int32_t>(a), b);
}

float SimpleFun2::operator() (const uint32_t a, const float b) const
{
    return (*this)(static_cast<float>(a), b);
}

double SimpleFun2::operator() (const uint32_t a, const double b) const
{
    return (*this)(static_cast<double>(a), b);
}

int32_t SimpleFun2::operator() (const int32_t a, const uint32_t b) const
{
    return (*this)(a, static_cast<int32_t>(b));
}

float SimpleFun2::operator() (const int32_t a, const float b) const
{
    return (*this)(static_cast<float>(a), b);
}

double SimpleFun2::operator() (const int32_t a, const double b) const
{
    return (*this)(static_cast<double>(a), b);
}

float SimpleFun2::operator() (const float a, const uint32_t b) const
{
    return (*this)(a, static_cast<float>(b));
}

float SimpleFun2::operator() (const float a, const int32_t b) const
{
    return (*this)(a, static_cast<float>(b));
}

double SimpleFun2::operator() (const float a, const double b) const
{
    return (*this)(static_cast<double>(a), b);
}

double SimpleFun2::operator() (const double a, const uint32_t b) const
{
    return (*this)(a, static_cast<double>(b));
}

double SimpleFun2::operator() (const double a, const int32_t b) const
{
    return (*this)(a, static_cast<double>(b));
}

double SimpleFun2::operator() (const double a, const float b) const
{
    return (*this)(a, static_cast<double>(b));
}

#define DEFN_SIMPLE_FUN_FP2(X, FUN32, FUN64) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return FUN32 (a, b); } \
\
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return FUN64 (a, b); }

#define DEFN_SIMPLE_FUN_INFIX2(X, FUN) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
\
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; } \
\
uint32_t _ ## X ## _ ::operator() (const uint32_t a, const uint32_t b) const { \
    return a FUN b; } \
\
int32_t _ ## X ## _ ::operator() (const int32_t a, const int32_t b) const { \
    return a FUN b; }

#define DEFN_SIMPLE_FUN_PREFIX2(X, FUN) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b; } \
\
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b; }

#define DEFN_SIMPLE_FUN_INTFIX2(X, FUN) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
uint32_t _ ## X ## _ ::operator() (const uint32_t a, const uint32_t b) const { \
    return a FUN b; } \
\
int32_t _ ## X ## _ ::operator() (const int32_t a, const int32_t b) const { \
    return a FUN b; }

_ABS_DIFF_ & _ABS_DIFF_::s(void)
{
    static _ABS_DIFF_ a;
    return a;
}
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

_ADD_SAT_ & _ADD_SAT_::s(void)
{
    static _ADD_SAT_ a;
    return a;
}
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

DEFN_SIMPLE_FUN_FP2(ATAN2, atan2f, atan2)

_ATAN2PI_ & _ATAN2PI_::s(void)
{
    static _ATAN2PI_ a;
    return a;
}
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
DEFN_SIMPLE_FUN_FP2(COPYSIGN, copysignf, copysign)
DEFN_SIMPLE_FUN_INFIX2(DIVIDE, /)
DEFN_SIMPLE_FUN_INFIX2(EQUAL, ==)
DEFN_SIMPLE_FUN_FP2(FDIM, fdimf, fdim)
DEFN_SIMPLE_FUN_FP2(FMAX, fmaxf, fmax)
DEFN_SIMPLE_FUN_FP2(FMIN, fminf, fmin)
DEFN_SIMPLE_FUN_FP2(FMOD, fmodf, fmod)
DEFN_SIMPLE_FUN_INFIX2(GREATEREQUAL, >=)
DEFN_SIMPLE_FUN_INFIX2(GREATERTHAN, >)

_HADD_ & _HADD_::s(void)
{
    static _HADD_ a;
    return a;
}
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

DEFN_SIMPLE_FUN_FP2(HYPOT, hypotf, hypot)
DEFN_SIMPLE_FUN_PREFIX2(ISEQUAL, ==)

_ISORDERED_ & _ISORDERED_::s(void)
{
    static _ISORDERED_ a;
    return a;
}
float _ISORDERED_::operator() (const float a, const float b) const
{
    return a == a && b == b;
}
double _ISORDERED_::operator() (const double a, const double b) const
{
    return a == a && b == b;
}

_ISUNORDERED_ & _ISUNORDERED_::s(void)
{
    static _ISUNORDERED_ a;
    return a;
}
float _ISUNORDERED_::operator() (const float a, const float b) const
{
    return isnan(a) || isnan(b);
}
double _ISUNORDERED_::operator() (const double a, const double b) const
{
    return isnan(a) || isnan(b);
}

DEFN_SIMPLE_FUN_PREFIX2(ISNOTEQUAL, !=)
DEFN_SIMPLE_FUN_PREFIX2(ISGREATER, >)
DEFN_SIMPLE_FUN_PREFIX2(ISGREATEREQUAL, >=)
DEFN_SIMPLE_FUN_PREFIX2(ISLESS, <)
DEFN_SIMPLE_FUN_PREFIX2(ISLESSEQUAL, <=)

_ISLESSGREATER_ & _ISLESSGREATER_::s(void)
{
    static _ISLESSGREATER_ a;
    return a;
}
float _ISLESSGREATER_::operator() (const float a, const float b) const
{
    return (a < b) || (a > b);
}
double _ISLESSGREATER_::operator() (const double a, const double b) const
{
    return (a < b) || (a > b);
}

DEFN_SIMPLE_FUN_FP2(LDEXP, ldexpf, ldexp)
DEFN_SIMPLE_FUN_INFIX2(LESSEQUAL, <=)
DEFN_SIMPLE_FUN_INFIX2(LESSTHAN, <)

DEFN_SIMPLE_FUN_INTFIX2(LOGICALAND, &&)
DEFN_SIMPLE_FUN_INTFIX2(LOGICALOR, ||)

_MAX_ & _MAX_::s(void)
{
    static _MAX_ a;
    return a;
}
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

_MAXMAG_ & _MAXMAG_::s(void)
{
    static _MAXMAG_ a;
    return a;
}
float _MAXMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) > fabsf(b))
        return a;
    else if (fabsf(a) < fabsf(b))
        return b;
    else
        return fmaxf(a, b);
}
double _MAXMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) > fabs(b))
        return a;
    else if (fabs(a) < fabs(b))
        return b;
    else
        return fmax(a, b);
}

_MIN_ & _MIN_::s(void)
{
    static _MIN_ a;
    return a;
}
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

_MINMAG_ & _MINMAG_::s(void)
{
    static _MINMAG_ a;
    return a;
}
float _MINMAG_::operator() (const float a, const float b) const
{
    if (fabsf(a) < fabsf(b))
        return a;
    else if (fabsf(a) > fabsf(b))
        return b;
    else
        return fminf(a, b);
}
double _MINMAG_::operator() (const double a, const double b) const
{
    if (fabs(a) < fabs(b))
        return a;
    else if (fabs(a) > fabs(b))
        return b;
    else
        return fmin(a, b);
}

DEFN_SIMPLE_FUN_INTFIX2(MODULO, %)
DEFN_SIMPLE_FUN_INFIX2(MULTIPLY, *)

_MUL24_ & _MUL24_::s(void)
{
    static _MUL24_ a;
    return a;
}
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

_MUL_HI_ & _MUL_HI_::s(void)
{
    static _MUL_HI_ a;
    return a;
}
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
DEFN_SIMPLE_FUN_FP2(NEXTAFTER, nextafterf, nextafter)

DEFN_SIMPLE_FUN_FP2(POW, powf, pow)

_POWN_ & _POWN_::s(void)
{
    static _POWN_ a;
    return a;
}
float _POWN_::operator() (const float a, const float b) const
{
    return powf(a, static_cast<int>(b));
}
double _POWN_::operator() (const double a, const double b) const
{
    return pow(a, static_cast<int>(b));
}

DEFN_SIMPLE_FUN_FP2(POWR, powf, pow)
DEFN_SIMPLE_FUN_FP2(REMAINDER, remainderf, remainder)

_RHADD_ & _RHADD_::s(void)
{
    static _RHADD_ a;
    return a;
}
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

_ROOTN_ & _ROOTN_::s(void)
{
    static _ROOTN_ a;
    return a;
}
float _ROOTN_::operator() (const float a, const float b) const
{
    return powf(a, 1.f / b);
}
double _ROOTN_::operator() (const double a, const double b) const
{
    return pow(a, 1.f / b);
}

_ROTATE_ & _ROTATE_::s(void)
{
    static _ROTATE_ a;
    return a;
}
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

_STEP_ & _STEP_::s(void)
{
    static _STEP_ a;
    return a;
}
float _STEP_::operator() (const float a, const float b) const
{
    return b < a ? 0.0 : 1.0;
}
double _STEP_::operator() (const double a, const double b) const
{
    return b < a ? 0.0 : 1.0;
}

DEFN_SIMPLE_FUN_INFIX2(SUBTRACT, -)

_SUB_SAT_ & _SUB_SAT_::s(void)
{
    static _SUB_SAT_ a;
    return a;
}
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

}; // namespace chai_internal
