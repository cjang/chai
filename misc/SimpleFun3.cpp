// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <stdlib.h>

#include "SimpleFun2.hpp"
#include "SimpleFun3.hpp"

using namespace std;

namespace chai_internal {

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

#define DEFN_SIMPLE_FUN_FP3(X, FUN32, FUN64) \
\
_ ## X ## _ & _ ## X ## _ ::s(void) { \
    static _ ## X ## _ a; \
    return a; } \
\
float _ ## X ## _ ::operator() (const float a, \
                                const float b, \
                                const float c) const { \
    return FUN32 (a, b, c); } \
\
double _ ## X ## _ ::operator() (const double a, \
                                 const double b, \
                                 const double c) const { \
    return FUN64 (a, b, c); }

_CLAMP_ & _CLAMP_::s(void)
{
    static _CLAMP_ a;
    return a;
}
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

DEFN_SIMPLE_FUN_FP3(FMA, fmaf, fma)

_MAD_ & _MAD_::s(void)
{
    static _MAD_ a;
    return a;
}
float _MAD_::operator() (const float a, const float b, const float c) const
{
    return a * b + c;
}
double _MAD_::operator() (const double a, const double b, const double c) const
{
    return a * b + c;
}

_MAD24_ & _MAD24_::s(void)
{
    static _MAD24_ a;
    return a;
}
uint32_t _MAD24_::operator() (const uint32_t a,
                              const uint32_t b,
                              const uint32_t c) const
{
    return _MUL24_::s()(a, b) + c;
}
int32_t _MAD24_::operator() (const int32_t a,
                             const int32_t b,
                             const int32_t c) const
{
    return _MUL24_::s()(a, b) + c;
}

_MAD_HI_ & _MAD_HI_::s(void)
{
    static _MAD_HI_ a;
    return a;
}
uint32_t _MAD_HI_::operator() (const uint32_t a,
                               const uint32_t b,
                               const uint32_t c) const
{
    return _MUL_HI_::s()(a, b) + c;
}
int32_t _MAD_HI_::operator() (const int32_t a,
                              const int32_t b,
                              const int32_t c) const
{
    return _MUL_HI_::s()(a, b) + c;
}

_MAD_SAT_ & _MAD_SAT_::s(void)
{
    static _MAD_SAT_ a;
    return a;
}
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

_MIX_ & _MIX_::s(void)
{
    static _MIX_ a;
    return a;
}
float _MIX_::operator() (const float a, const float b, const float c) const
{
    return a + (b - a) * c;
}
double _MIX_::operator() (const double a, const double b, const double c) const
{
    return a + (b - a) * c;
}

_SMOOTHSTEP_ & _SMOOTHSTEP_::s(void)
{
    static _SMOOTHSTEP_ a;
    return a;
}
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
