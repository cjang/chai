// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SIMPLE_FUN3_HPP_
#define _CHAI_SIMPLE_FUN3_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// functions with three arguments

// floating point
struct SimpleFun3
{
    virtual ~SimpleFun3(void);

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
    static _ ## X ## _ & s(void); \
    float operator() (const float, const float, const float) const; \
    double operator() (const double, const double, const double) const; \
};

#define DECL_SIMPLE_FUN_IU3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & s(void); \
    int32_t operator() (const int32_t, const int32_t, const int32_t) const; \
    uint32_t operator() (const uint32_t, const uint32_t, const uint32_t) const; \
};

#define DECL_SIMPLE_FUN_FPIU3(X) \
struct _ ## X ## _ : public SimpleFun3 { \
    static _ ## X ## _ & s(void); \
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
