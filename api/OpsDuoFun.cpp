// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// binary operation

DuoFun operator&& (const ArrayExp& A, const ArrayExp& B) {
    return DuoFun(ByteCodes::operatorLOGICALAND, A, B); }

DuoFun operator|| (const ArrayExp& A, const ArrayExp& B) {
    return DuoFun(ByteCodes::operatorLOGICALOR, A, B); }

#define DEFN_ARRAY_BINOPFUN(NAME, FUNCODE) \
DuoFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const ArrayExp& A, const uint32_t B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const uint32_t A, const ArrayExp& B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const ArrayExp& A, const int32_t B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const int32_t A, const ArrayExp& B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const ArrayExp& A, const float B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const float A, const ArrayExp& B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const ArrayExp& A, const double B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); } \
DuoFun NAME (const double A, const ArrayExp& B) { \
    return DuoFun(ByteCodes:: FUNCODE, A, B); }

DEFN_ARRAY_BINOPFUN(operator==, operatorEQUAL)
DEFN_ARRAY_BINOPFUN(operator!=, operatorNOTEQUAL)
DEFN_ARRAY_BINOPFUN(operator<=, operatorLESSEQUAL)
DEFN_ARRAY_BINOPFUN(operator>=, operatorGREATEREQUAL)
DEFN_ARRAY_BINOPFUN(operator<, operatorLESSTHAN)
DEFN_ARRAY_BINOPFUN(operator>, operatorGREATERTHAN)
DEFN_ARRAY_BINOPFUN(abs_diff, abs_diff)
DEFN_ARRAY_BINOPFUN(add_sat, add_sat)
DEFN_ARRAY_BINOPFUN(hadd, hadd)
DEFN_ARRAY_BINOPFUN(mul24, mul24)
DEFN_ARRAY_BINOPFUN(mul_hi, mul_hi)
DEFN_ARRAY_BINOPFUN(rhadd, rhadd)
DEFN_ARRAY_BINOPFUN(rotate, rotate)
DEFN_ARRAY_BINOPFUN(sub_sat, sub_sat)
DEFN_ARRAY_BINOPFUN(operator+, operatorADD)
DEFN_ARRAY_BINOPFUN(operator-, operatorSUBTRACT)
DEFN_ARRAY_BINOPFUN(operator*, operatorMULTIPLY)
DEFN_ARRAY_BINOPFUN(operator/, operatorDIVIDE)
DEFN_ARRAY_BINOPFUN(operator%, operatorMODULO)
DEFN_ARRAY_BINOPFUN(operator<<, operatorSHIFTLEFT)
DEFN_ARRAY_BINOPFUN(operator>>, operatorSHIFTRIGHT)
DEFN_ARRAY_BINOPFUN(operator&, operatorBITWISEAND)
DEFN_ARRAY_BINOPFUN(operator|, operatorBITWISEOR)
DEFN_ARRAY_BINOPFUN(operator^, operatorBITWISEXOR)
DEFN_ARRAY_BINOPFUN(max, max)
DEFN_ARRAY_BINOPFUN(min, min)
DEFN_ARRAY_BINOPFUN(maxmag, maxmag)
DEFN_ARRAY_BINOPFUN(minmag, minmag)
DEFN_ARRAY_BINOPFUN(nextafter, nextafter)
DEFN_ARRAY_BINOPFUN(pow, pow)
DEFN_ARRAY_BINOPFUN(pown, pown)
DEFN_ARRAY_BINOPFUN(powr, powr)
DEFN_ARRAY_BINOPFUN(remainder, remainder)
DEFN_ARRAY_BINOPFUN(rootn, rootn)
DEFN_ARRAY_BINOPFUN(step, step)
DEFN_ARRAY_BINOPFUN(atan2, atan2)
DEFN_ARRAY_BINOPFUN(atan2pi, atan2pi)
DEFN_ARRAY_BINOPFUN(copysign, copysign)
DEFN_ARRAY_BINOPFUN(fdim, fdim)
DEFN_ARRAY_BINOPFUN(fmax, fmax)
DEFN_ARRAY_BINOPFUN(fmin, fmin)
DEFN_ARRAY_BINOPFUN(fmod, fmod)
DEFN_ARRAY_BINOPFUN(hypot, hypot)
DEFN_ARRAY_BINOPFUN(ldexp, ldexp)
DEFN_ARRAY_BINOPFUN(isequal, isequal)
DEFN_ARRAY_BINOPFUN(isnotequal, isnotequal)
DEFN_ARRAY_BINOPFUN(isgreater, isgreater)
DEFN_ARRAY_BINOPFUN(isgreaterequal, isgreaterequal)
DEFN_ARRAY_BINOPFUN(isless, isless)
DEFN_ARRAY_BINOPFUN(islessequal, islessequal)
DEFN_ARRAY_BINOPFUN(islessgreater, islessgreater)

}; // namespace chai
