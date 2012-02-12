// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SIMPLE_FUNS_HPP_
#define _CHAI_SIMPLE_FUNS_HPP_

#include <string>

namespace chai_internal {

////////////////////////////////////////
// unary functions

struct SimpleFun1 {
    virtual ~SimpleFun1(void);
    virtual std::string str(void) const = 0;
    virtual float operator() (const float) const = 0;
    virtual double operator() (const double) const = 0;
};

#define DECL_SIMPLE_FUN1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float) const; \
    double operator() (const double) const; \
};

// all isomorphic w.r.t. array stream dimensions
DECL_SIMPLE_FUN1(ACOS)
DECL_SIMPLE_FUN1(ACOSH)
DECL_SIMPLE_FUN1(ACOSPI)
DECL_SIMPLE_FUN1(ASIN)
DECL_SIMPLE_FUN1(ASINH)
DECL_SIMPLE_FUN1(ASINPI)
DECL_SIMPLE_FUN1(ATAN)
DECL_SIMPLE_FUN1(ATANH)
DECL_SIMPLE_FUN1(ATANPI)
DECL_SIMPLE_FUN1(CBRT)
DECL_SIMPLE_FUN1(CEIL)
DECL_SIMPLE_FUN1(COS)
DECL_SIMPLE_FUN1(COSH)
DECL_SIMPLE_FUN1(COSPI)
DECL_SIMPLE_FUN1(DEGREES)
DECL_SIMPLE_FUN1(ERFC)
DECL_SIMPLE_FUN1(ERF)
DECL_SIMPLE_FUN1(EXP)
DECL_SIMPLE_FUN1(EXP2)
DECL_SIMPLE_FUN1(EXP10)
DECL_SIMPLE_FUN1(EXPM1)
DECL_SIMPLE_FUN1(FABS)
DECL_SIMPLE_FUN1(FLOOR)
DECL_SIMPLE_FUN1(ILOGB)
DECL_SIMPLE_FUN1(ISFINITE)
DECL_SIMPLE_FUN1(ISINF)
DECL_SIMPLE_FUN1(ISNAN)
DECL_SIMPLE_FUN1(ISNORMAL)
DECL_SIMPLE_FUN1(LGAMMA)
DECL_SIMPLE_FUN1(LOG)
DECL_SIMPLE_FUN1(LOG2)
DECL_SIMPLE_FUN1(LOG10)
DECL_SIMPLE_FUN1(LOG1P)
DECL_SIMPLE_FUN1(LOGB)
DECL_SIMPLE_FUN1(NAN)
DECL_SIMPLE_FUN1(NEGATE)
DECL_SIMPLE_FUN1(RADIANS)
DECL_SIMPLE_FUN1(RINT)
DECL_SIMPLE_FUN1(ROUND)
DECL_SIMPLE_FUN1(RSQRT)
DECL_SIMPLE_FUN1(SIGN)
DECL_SIMPLE_FUN1(SIGNBIT)
DECL_SIMPLE_FUN1(SIN)
DECL_SIMPLE_FUN1(SINH)
DECL_SIMPLE_FUN1(SINPI)
DECL_SIMPLE_FUN1(SQRT)
DECL_SIMPLE_FUN1(TAN)
DECL_SIMPLE_FUN1(TANH)
DECL_SIMPLE_FUN1(TANPI)
DECL_SIMPLE_FUN1(TGAMMA)
DECL_SIMPLE_FUN1(TRUNC)

////////////////////////////////////////
// binary operators and functions

struct SimpleFun2 {
    virtual ~SimpleFun2(void);
    virtual bool infix(void) const = 0;
    virtual std::string str(void) const = 0;
    virtual float operator() (const float, const float) const = 0;
    virtual double operator() (const float, const double) const = 0;
    virtual double operator() (const double, const float) const = 0;
    virtual double operator() (const double, const double) const = 0;
};

#define DECL_SIMPLE_FUN2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & singleton(void); \
    bool infix(void) const; \
    std::string str(void) const; \
    float operator() (const float, const float) const; \
    double operator() (const float, const double) const; \
    double operator() (const double, const float) const; \
    double operator() (const double, const double) const; \
};

// binary infix operators
DECL_SIMPLE_FUN2(ADD)
DECL_SIMPLE_FUN2(AND)
DECL_SIMPLE_FUN2(DIV)
DECL_SIMPLE_FUN2(EQ)
DECL_SIMPLE_FUN2(GE)
DECL_SIMPLE_FUN2(GT)
DECL_SIMPLE_FUN2(LE)
DECL_SIMPLE_FUN2(LT)
DECL_SIMPLE_FUN2(MUL)
DECL_SIMPLE_FUN2(NE)
DECL_SIMPLE_FUN2(OR)
DECL_SIMPLE_FUN2(SUB)

// prefixed function calls, not infix
DECL_SIMPLE_FUN2(ISORDERED)
DECL_SIMPLE_FUN2(ISUNORDERED)
DECL_SIMPLE_FUN2(MAX)
DECL_SIMPLE_FUN2(MIN)
DECL_SIMPLE_FUN2(MAXMAG)
DECL_SIMPLE_FUN2(MINMAG)
DECL_SIMPLE_FUN2(NEXTAFTER)
DECL_SIMPLE_FUN2(POW)
DECL_SIMPLE_FUN2(POWN)
DECL_SIMPLE_FUN2(POWR)
DECL_SIMPLE_FUN2(REMAINDER)
DECL_SIMPLE_FUN2(ROOTN)
DECL_SIMPLE_FUN2(STEP)
DECL_SIMPLE_FUN2(ATAN2)
DECL_SIMPLE_FUN2(ATAN2PI)
DECL_SIMPLE_FUN2(COPYSIGN)
DECL_SIMPLE_FUN2(FDIM)
DECL_SIMPLE_FUN2(FMAX)
DECL_SIMPLE_FUN2(FMIN)
DECL_SIMPLE_FUN2(FMOD)
DECL_SIMPLE_FUN2(HYPOT)
DECL_SIMPLE_FUN2(LDEXP)
DECL_SIMPLE_FUN2(ISEQUAL)
DECL_SIMPLE_FUN2(ISNOTEQUAL)
DECL_SIMPLE_FUN2(ISGREATER)
DECL_SIMPLE_FUN2(ISGREATEREQUAL)
DECL_SIMPLE_FUN2(ISLESS)
DECL_SIMPLE_FUN2(ISLESSEQUAL)
DECL_SIMPLE_FUN2(ISLESSGREATER)

////////////////////////////////////////
// functions with three arguments

struct SimpleFun3 {
    virtual ~SimpleFun3(void);
    virtual std::string str(void) const = 0;
    virtual float operator() (const float,
                              const float,
                              const float) const = 0;
    virtual double operator() (const double,
                               const double,
                               const double) const = 0;
    double operator() (const float, const float, const double) const;
    double operator() (const float, const double, const float) const;
    double operator() (const float, const double, const double) const;
    double operator() (const double, const float, const float) const;
    double operator() (const double, const float, const double) const;
    double operator() (const double, const double, const float) const;
};

#define DECL_SIMPLE_FUN3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float, const float, const float) const; \
    double operator() (const double, const double, const double) const; \
};

DECL_SIMPLE_FUN3(FMA)
DECL_SIMPLE_FUN3(MAD)
DECL_SIMPLE_FUN3(CLAMP)
DECL_SIMPLE_FUN3(MIX)
DECL_SIMPLE_FUN3(SMOOTHSTEP)

}; // namespace chai_internal

#endif
