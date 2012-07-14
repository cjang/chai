// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SIMPLE_FUN1_HPP_
#define _CHAI_SIMPLE_FUN1_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// unary functions

struct SimpleFun1
{
    virtual ~SimpleFun1(void);

    virtual uint32_t operator() (const uint32_t) const;
    virtual int32_t operator() (const int32_t) const;
    virtual float operator() (const float) const;
    virtual double operator() (const double) const;
};

#define DECL_SIMPLE_FUN_FP1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & s(void); \
    float operator() (const float) const; \
    double operator() (const double) const; \
};

#define DECL_SIMPLE_FUN_IU1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & s(void); \
    int32_t operator() (const int32_t) const; \
    uint32_t operator() (const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPI1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & s(void); \
    float operator() (const float) const; \
    double operator() (const double) const; \
    int32_t operator() (const int32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & s(void); \
    float operator() (const float) const; \
    double operator() (const double) const; \
    int32_t operator() (const int32_t) const; \
    uint32_t operator() (const uint32_t) const; \
};

DECL_SIMPLE_FUN_IU1(ABS)
DECL_SIMPLE_FUN_FP1(ACOS)
DECL_SIMPLE_FUN_FP1(ACOSH)
DECL_SIMPLE_FUN_FP1(ACOSPI)
DECL_SIMPLE_FUN_FP1(ASIN)
DECL_SIMPLE_FUN_FP1(ASINH)
DECL_SIMPLE_FUN_FP1(ASINPI)
DECL_SIMPLE_FUN_FP1(ATAN)
DECL_SIMPLE_FUN_FP1(ATANH)
DECL_SIMPLE_FUN_FP1(ATANPI)
DECL_SIMPLE_FUN_FP1(CBRT)
DECL_SIMPLE_FUN_FP1(CEIL)
DECL_SIMPLE_FUN_IU1(CLZ)
DECL_SIMPLE_FUN_IU1(COMPLEMENT)
DECL_SIMPLE_FUN_FP1(COS)
DECL_SIMPLE_FUN_FP1(COSH)
DECL_SIMPLE_FUN_FP1(COSPI)
DECL_SIMPLE_FUN_FP1(DEGREES)
DECL_SIMPLE_FUN_FP1(ERFC)
DECL_SIMPLE_FUN_FP1(ERF)
DECL_SIMPLE_FUN_FP1(EXP)
DECL_SIMPLE_FUN_FP1(EXP2)
DECL_SIMPLE_FUN_FP1(EXP10)
DECL_SIMPLE_FUN_FP1(EXPM1)
DECL_SIMPLE_FUN_FP1(FABS)
DECL_SIMPLE_FUN_FPIU1(FLOOR)
DECL_SIMPLE_FUN_FP1(ILOGB)
DECL_SIMPLE_FUN_FP1(ISFINITE)
DECL_SIMPLE_FUN_FP1(ISINF)
DECL_SIMPLE_FUN_FP1(ISNAN)
DECL_SIMPLE_FUN_FP1(ISNORMAL)
DECL_SIMPLE_FUN_FP1(LGAMMA)
DECL_SIMPLE_FUN_FP1(LOG)
DECL_SIMPLE_FUN_FP1(LOG2)
DECL_SIMPLE_FUN_FP1(LOG10)
DECL_SIMPLE_FUN_FP1(LOG1P)
DECL_SIMPLE_FUN_FP1(LOGB)
DECL_SIMPLE_FUN_FP1(NAN)
DECL_SIMPLE_FUN_IU1(NOT)
DECL_SIMPLE_FUN_FP1(RADIANS)
DECL_SIMPLE_FUN_FP1(RINT)
DECL_SIMPLE_FUN_FP1(ROUND)
DECL_SIMPLE_FUN_FP1(RSQRT)
DECL_SIMPLE_FUN_FP1(SIGN)
DECL_SIMPLE_FUN_FP1(SIGNBIT)
DECL_SIMPLE_FUN_FP1(SIN)
DECL_SIMPLE_FUN_FP1(SINH)
DECL_SIMPLE_FUN_FP1(SINPI)
DECL_SIMPLE_FUN_FP1(SQRT)
DECL_SIMPLE_FUN_FP1(TAN)
DECL_SIMPLE_FUN_FP1(TANH)
DECL_SIMPLE_FUN_FP1(TANPI)
DECL_SIMPLE_FUN_FP1(TGAMMA)
DECL_SIMPLE_FUN_FP1(TRUNC)
DECL_SIMPLE_FUN_FPI1(UNARYMINUS)

}; // namespace chai_internal

#endif
