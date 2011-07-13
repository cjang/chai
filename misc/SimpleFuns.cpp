// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>

#include "SimpleFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// unary functions

SimpleFun1::~SimpleFun1(void) { }

#define DEFN_SIMPLE_FUN1(X, NAME, FUN32, FUN64) \
_ ## X ## _ & _ ## X ## _ ::obj(void) { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a) const { return FUN32 (a); } \
double _ ## X ## _ ::operator() (const double a) const { return FUN64 (a); }

DEFN_SIMPLE_FUN1(ABS, fabs, fabsf, fabs)
DEFN_SIMPLE_FUN1(EXP, exp, expf, exp)
DEFN_SIMPLE_FUN1(NEGATE, -, -, -)
DEFN_SIMPLE_FUN1(SQRT, sqrt, sqrtf, sqrt)
DEFN_SIMPLE_FUN1(FLOOR, floor, floorf, floor)

////////////////////////////////////////
// binary operators and functions

SimpleFun2::~SimpleFun2(void) { }

#define DEFN_SIMPLE_BINOP(X, FUN) \
_ ## X ## _ & _ ## X ## _ ::obj(void) { static _ ## X ## _ a; return a; } \
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

#define DEFN_SIMPLE_TERNARY(X, NAME, FUN) \
_ ## X ## _ & _ ## X ## _ ::obj(void) { static _ ## X ## _ a; return a; } \
string _ ## X ## _ ::str(void) const { return #NAME; } \
float _ ## X ## _ ::operator() (const float a, const float b) const { \
    return a FUN b ? a : b; } \
double _ ## X ## _ ::operator() (const float a, const double b) const { \
    return a FUN b ? a : b; } \
double _ ## X ## _ ::operator() (const double a, const float b) const { \
    return a FUN b ? a : b; } \
double _ ## X ## _ ::operator() (const double a, const double b) const { \
    return a FUN b ? a : b; }

DEFN_SIMPLE_TERNARY(MAX, fmax, >)
DEFN_SIMPLE_TERNARY(MIN, fmin, <)

}; // namespace chai_internal
