// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
    static _ ## X ## _ & obj(void); \
    std::string str(void) const; \
    float operator() (const float) const; \
    double operator() (const double) const; \
};

DECL_SIMPLE_FUN1(ABS)
DECL_SIMPLE_FUN1(EXP)
DECL_SIMPLE_FUN1(NEGATE)
DECL_SIMPLE_FUN1(SQRT)
DECL_SIMPLE_FUN1(FLOOR)

////////////////////////////////////////
// binary operators and functions

struct SimpleFun2 {
    virtual ~SimpleFun2(void);
    virtual std::string str(void) const = 0;
    virtual float operator() (const float, const float) const = 0;
    virtual double operator() (const float, const double) const = 0;
    virtual double operator() (const double, const float) const = 0;
    virtual double operator() (const double, const double) const = 0;
};

#define DECL_SIMPLE_FUN2(X) \
struct _ ## X ## _ : public SimpleFun2 { \
    static _ ## X ## _ & obj(void); \
    std::string str(void) const; \
    float operator() (const float, const float) const; \
    double operator() (const float, const double) const; \
    double operator() (const double, const float) const; \
    double operator() (const double, const double) const; \
};

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
DECL_SIMPLE_FUN2(MAX)
DECL_SIMPLE_FUN2(MIN)

}; // namespace chai_internal

#endif
