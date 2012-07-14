// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SIMPLE_FUN2_HPP_
#define _CHAI_SIMPLE_FUN2_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// binary operators and functions

struct SimpleFun2
{
    virtual ~SimpleFun2(void);

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
    static _ ## X ## _ & s(void); \
    float operator() (const float, const float) const; \
    double operator() (const double, const double) const; \
};

#define DECL_SIMPLE_FUN_IU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & s(void); \
    int32_t operator() (const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & s(void); \
    float operator() (const float, const float) const; \
    double operator() (const double, const double) const; \
    int32_t operator() (const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_INTU2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & s(void); \
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

}; // namespace chai_internal

#endif
