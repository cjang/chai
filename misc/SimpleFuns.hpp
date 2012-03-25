// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SIMPLE_FUNS_HPP_
#define _CHAI_SIMPLE_FUNS_HPP_

#include <stdint.h>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// unary functions

struct SimpleFun1
{
    virtual ~SimpleFun1(void);
    virtual std::string str(void) const = 0;

    virtual uint32_t operator() (const uint32_t) const;
    virtual int32_t operator() (const int32_t) const;
    virtual float operator() (const float) const;
    virtual double operator() (const double) const;
};

#define DECL_SIMPLE_FUN_FP1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float) const; \
    double operator() (const double) const; \
};

#define DECL_SIMPLE_FUN_IU1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    int32_t operator() (const int32_t) const; \
    uint32_t operator() (const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPI1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float) const; \
    double operator() (const double) const; \
    int32_t operator() (const int32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU1(X) \
struct _ ## X ## _ : public SimpleFun1 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
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

////////////////////////////////////////
// binary operators and functions

struct SimpleFun2
{
    virtual ~SimpleFun2(void);
    virtual std::string str(void) const = 0;
    virtual bool infix(void) const;

    virtual uint32_t operator() (const uint32_t, const uint32_t) const;
    virtual int32_t operator() (const int32_t, const int32_t) const;
    virtual float operator() (const float, const float) const;
    virtual double operator() (const double, const double) const;

    int32_t operator() (const uint32_t, const int32_t) const;
    float operator() (const uint32_t, const float) const;
    double operator() (const uint32_t, const double) const;
    int32_t operator() (const int32_t, const uint32_t) const;
    float operator() (const int32_t, const float) const;
    double operator() (const int32_t, const double) const;
    float operator() (const float, const uint32_t) const;
    float operator() (const float, const int32_t) const;
    double operator() (const float, const double) const;
    double operator() (const double, const uint32_t) const;
    double operator() (const double, const int32_t) const;
    double operator() (const double, const float) const;
};

#define DECL_SIMPLE_FUN_FP2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float, const float) const; \
    double operator() (const double, const double) const; \
};

#define DECL_SIMPLE_FUN_IU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    int32_t operator() (const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & singleton(void); \
    bool infix(void) const; \
    std::string str(void) const; \
    float operator() (const float, const float) const; \
    double operator() (const double, const double) const; \
    int32_t operator() (const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_INTU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & singleton(void); \
    bool infix(void) const; \
    std::string str(void) const; \
    int32_t operator() (const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t) const; \
};

DECL_SIMPLE_FUN_IU2(ABS_DIFF)
DECL_SIMPLE_FUN_FPIU2(ADD)
DECL_SIMPLE_FUN_IU2(ADD_SAT)
DECL_SIMPLE_FUN_FP2(ATAN2)
DECL_SIMPLE_FUN_FP2(ATAN2PI)
DECL_SIMPLE_FUN_INTU2(BITWISEAND)
DECL_SIMPLE_FUN_INTU2(BITWISEOR)
DECL_SIMPLE_FUN_INTU2(BITWISEXOR)
DECL_SIMPLE_FUN_FP2(COPYSIGN)
DECL_SIMPLE_FUN_FPIU2(DIVIDE)
DECL_SIMPLE_FUN_FPIU2(EQUAL)
DECL_SIMPLE_FUN_FP2(FDIM)
DECL_SIMPLE_FUN_FP2(FMAX)
DECL_SIMPLE_FUN_FP2(FMIN)
DECL_SIMPLE_FUN_FP2(FMOD)
DECL_SIMPLE_FUN_FPIU2(GREATEREQUAL)
DECL_SIMPLE_FUN_FPIU2(GREATERTHAN)
DECL_SIMPLE_FUN_IU2(HADD)
DECL_SIMPLE_FUN_FP2(HYPOT)
DECL_SIMPLE_FUN_FP2(ISEQUAL)
DECL_SIMPLE_FUN_FP2(ISORDERED)
DECL_SIMPLE_FUN_FP2(ISUNORDERED)
DECL_SIMPLE_FUN_FP2(ISNOTEQUAL)
DECL_SIMPLE_FUN_FP2(ISGREATER)
DECL_SIMPLE_FUN_FP2(ISGREATEREQUAL)
DECL_SIMPLE_FUN_FP2(ISLESS)
DECL_SIMPLE_FUN_FP2(ISLESSEQUAL)
DECL_SIMPLE_FUN_FP2(ISLESSGREATER)
DECL_SIMPLE_FUN_FP2(LDEXP)
DECL_SIMPLE_FUN_FPIU2(LESSEQUAL)
DECL_SIMPLE_FUN_FPIU2(LESSTHAN)
DECL_SIMPLE_FUN_INTU2(LOGICALAND)
DECL_SIMPLE_FUN_INTU2(LOGICALOR)
DECL_SIMPLE_FUN_FPIU2(MAX)
DECL_SIMPLE_FUN_FP2(MAXMAG)
DECL_SIMPLE_FUN_FPIU2(MIN)
DECL_SIMPLE_FUN_FP2(MINMAG)
DECL_SIMPLE_FUN_INTU2(MODULO)
DECL_SIMPLE_FUN_FPIU2(MULTIPLY)
DECL_SIMPLE_FUN_IU2(MUL24)
DECL_SIMPLE_FUN_IU2(MUL_HI)
DECL_SIMPLE_FUN_FPIU2(NOTEQUAL)
DECL_SIMPLE_FUN_FP2(NEXTAFTER)
DECL_SIMPLE_FUN_FP2(POW)
DECL_SIMPLE_FUN_FP2(POWN)
DECL_SIMPLE_FUN_FP2(POWR)
DECL_SIMPLE_FUN_FP2(REMAINDER)
DECL_SIMPLE_FUN_IU2(RHADD)
DECL_SIMPLE_FUN_FP2(ROOTN)
DECL_SIMPLE_FUN_IU2(ROTATE)
DECL_SIMPLE_FUN_INTU2(SHIFTLEFT)
DECL_SIMPLE_FUN_INTU2(SHIFTRIGHT)
DECL_SIMPLE_FUN_FP2(STEP)
DECL_SIMPLE_FUN_FPIU2(SUBTRACT)
DECL_SIMPLE_FUN_IU2(SUB_SAT)

////////////////////////////////////////
// functions with three arguments

// floating point
struct SimpleFun3
{
    virtual ~SimpleFun3(void);
    virtual std::string str(void) const = 0;

    virtual uint32_t operator() (const uint32_t,
                                 const uint32_t,
                                 const uint32_t) const;

    virtual int32_t operator() (const int32_t,
                                const int32_t,
                                const int32_t) const;

    virtual float operator() (const float,
                              const float,
                              const float) const;

    virtual double operator() (const double,
                               const double,
                               const double) const;

    int32_t operator() (const uint32_t, const uint32_t, const int32_t) const;
    float operator() (const uint32_t, const uint32_t, const float) const;
    double operator() (const uint32_t, const uint32_t, const double) const;

    int32_t operator() (const uint32_t, const int32_t, const uint32_t) const;
    int32_t operator() (const uint32_t, const int32_t, const int32_t) const;
    float operator() (const uint32_t, const int32_t, const float) const;
    double operator() (const uint32_t, const int32_t, const double) const;

    float operator() (const uint32_t, const float, const uint32_t) const;
    float operator() (const uint32_t, const float, const int32_t) const;
    float operator() (const uint32_t, const float, const float) const;
    double operator() (const uint32_t, const float, const double) const;

    double operator() (const uint32_t, const double, const uint32_t) const;
    double operator() (const uint32_t, const double, const int32_t) const;
    double operator() (const uint32_t, const double, const float) const;
    double operator() (const uint32_t, const double, const double) const;

    int32_t operator() (const int32_t, const uint32_t, const uint32_t) const;
    int32_t operator() (const int32_t, const uint32_t, const int32_t) const;
    float operator() (const int32_t, const uint32_t, const float) const;
    double operator() (const int32_t, const uint32_t, const double) const;

    int32_t operator() (const int32_t, const int32_t, const uint32_t) const;
    float operator() (const int32_t, const int32_t, const float) const;
    double operator() (const int32_t, const int32_t, const double) const;

    float operator() (const int32_t, const float, const uint32_t) const;
    float operator() (const int32_t, const float, const int32_t) const;
    float operator() (const int32_t, const float, const float) const;
    double operator() (const int32_t, const float, const double) const;

    double operator() (const int32_t, const double, const uint32_t) const;
    double operator() (const int32_t, const double, const int32_t) const;
    double operator() (const int32_t, const double, const float) const;
    double operator() (const int32_t, const double, const double) const;

    float operator() (const float, const uint32_t, const uint32_t) const;
    float operator() (const float, const uint32_t, const int32_t) const;
    float operator() (const float, const uint32_t, const float) const;
    double operator() (const float, const uint32_t, const double) const;

    float operator() (const float, const int32_t, const uint32_t) const;
    float operator() (const float, const int32_t, const int32_t) const;
    float operator() (const float, const int32_t, const float) const;
    double operator() (const float, const int32_t, const double) const;

    float operator() (const float, const float, const uint32_t) const;
    float operator() (const float, const float, const int32_t) const;
    double operator() (const float, const float, const double) const;

    double operator() (const float, const double, const uint32_t) const;
    double operator() (const float, const double, const int32_t) const;
    double operator() (const float, const double, const float) const;
    double operator() (const float, const double, const double) const;

    double operator() (const double, const uint32_t, const uint32_t) const;
    double operator() (const double, const uint32_t, const int32_t) const;
    double operator() (const double, const uint32_t, const float) const;
    double operator() (const double, const uint32_t, const double) const;

    double operator() (const double, const int32_t, const uint32_t) const;
    double operator() (const double, const int32_t, const int32_t) const;
    double operator() (const double, const int32_t, const float) const;
    double operator() (const double, const int32_t, const double) const;

    double operator() (const double, const float, const uint32_t) const;
    double operator() (const double, const float, const int32_t) const;
    double operator() (const double, const float, const float) const;
    double operator() (const double, const float, const double) const;

    double operator() (const double, const double, const uint32_t) const;
    double operator() (const double, const double, const int32_t) const;
    double operator() (const double, const double, const float) const;
};

#define DECL_SIMPLE_FUN_FP3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float, const float, const float) const; \
    double operator() (const double, const double, const double) const; \
};

#define DECL_SIMPLE_FUN_IU3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    int32_t operator() (const int32_t, const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & singleton(void); \
    std::string str(void) const; \
    float operator() (const float, const float, const float) const; \
    double operator() (const double, const double, const double) const; \
    int32_t operator() (const int32_t, const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t, const uint32_t) const; \
};

DECL_SIMPLE_FUN_FPIU3(CLAMP)
DECL_SIMPLE_FUN_FP3(FMA)
DECL_SIMPLE_FUN_FP3(MAD)
DECL_SIMPLE_FUN_IU3(MAD24)
DECL_SIMPLE_FUN_IU3(MAD_HI)
DECL_SIMPLE_FUN_IU3(MAD_SAT)
DECL_SIMPLE_FUN_FP3(MIX)
DECL_SIMPLE_FUN_FP3(SMOOTHSTEP)

}; // namespace chai_internal

#endif
