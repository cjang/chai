// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>
#include <stdlib.h>
#include <string.h>

#include "ArrayClient.hpp"
#include "ByteCodes.hpp"
#include "chai/chai.h"
#include "FrontMem.hpp"
#include "PrecType.hpp"
#include "Scheduler.hpp"
#include "SingleNut.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// virtual machine management

void init(istream& configSpecStream)
{
    Scheduler::singleton().init(configSpecStream);
}

void init(const string& configSpecFileName)
{
    ifstream configSpecStream(configSpecFileName.c_str());
    init(configSpecStream);
}

void shutdown(void)
{
    Scheduler::singleton().shutdown();
}

////////////////////////////////////////
// array expression interface

ArrayDim::ArrayDim(const size_t w, const size_t h)
    : width(w),
      height(h),
      slots(1) { }

ArrayDim::ArrayDim(const size_t w, const size_t h, const size_t s)
    : width(w),
      height(h),
      slots(s) { }

ArrayExp::~ArrayExp(void) { }

////////////////////////////////////////
// array literals

#define DEFN_ARRAY_LIT(FP, X) \
ArrayLit ## FP :: ArrayLit ## FP (const X a) : _lit(a) { } \
X ArrayLit ## FP ::value(void) const { return _lit; } \
void ArrayLit ## FP ::accept(Stak<BC>& v) const { \
    v.push(static_cast<double>(_lit)); \
    v.push(ByteCodes::scalar_ ## FP); } \
void ArrayLit ## FP ::accept(stack< ArrayDim >& v) const { \
    v.push(ArrayDim(1, 1)); }

DEFN_ARRAY_LIT(u32, uint32_t)
DEFN_ARRAY_LIT(i32, int32_t)
DEFN_ARRAY_LIT(f32, float)
DEFN_ARRAY_LIT(f64, double)

////////////////////////////////////////
// array functions

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); }

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const ArrayExp& a2,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); arg(a2); }

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const ArrayExp& a2,
                   const ArrayExp& a3,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); arg(a2); arg(a3); }

ArrayFun::ArrayFun(const ArrayFun& other)
    : _funCode(other._funCode),
      _dimStack(other._dimStack),
      _argList(other._argList) { }

uint32_t ArrayFun::code(void) const { return _funCode; }

void ArrayFun::arg(const ArrayExp& a) { _argList.push_back(&a); }

void ArrayFun::accept(Stak<BC>& v) const
{
    for (vector<const ArrayExp*>::const_reverse_iterator
         it = _argList.rbegin();
         it != _argList.rend();
         it++) { (*it)->accept(v); }

    v.push(_funCode);
}

void ArrayFun::accept(stack< ArrayDim >& v) const
{
    for (vector<const ArrayExp*>::const_reverse_iterator
         it = _argList.rbegin();
         it != _argList.rend();
         it++) { (*it)->accept(v); }

    switch (_dimStack)
    {
        case (UNCHANGED) :
            break;

        case (REDUCED) :
            {
            ArrayDim a(1, 1, v.top().slots); // reduction to scalar
            v.pop();
            v.push(a);
            }
            break;

        case (MATMUL) :
            {
            const ArrayDim l = v.top();
            v.pop();
            const ArrayDim r = v.top();
            v.pop();
            const size_t maxslots = l.slots > r.slots ? l.slots : r.slots;
            if (1 == l.height && 1 == r.height)
                v.push(ArrayDim(r.width, l.width, maxslots));  // outer product
            else if (1 == l.height)
                v.push(ArrayDim(r.width, 1, maxslots));        // vector-matrix
            else if (1 == r.height)
                v.push(ArrayDim(l.height, 1, maxslots));       // matrix-vector
            else
                v.push(ArrayDim(r.width, l.height, maxslots)); // matrix-matrix
            }
            break;

        case (FIRST2) :
            {
            const ArrayDim a = v.top(); // first argument of two
            v.pop();
            v.pop();
            v.push(a);
            }
            break;

        case (FIRST3) :
            {
            const ArrayDim a = v.top(); // first argument of three
            v.pop();
            v.pop();
            v.pop();
            v.push(a);
            }
            break;

        case (TRANSPOSED) :
            {
            const ArrayDim s = v.top();
            v.pop();
            v.push(ArrayDim(s.height, s.width, s.slots)); // swap width, height
            }
            break;
    }
}

#define DEFN_ARRAY_FUN1(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN1(abs, abs, UNCHANGED)
DEFN_ARRAY_FUN1(acos, acos, UNCHANGED)
DEFN_ARRAY_FUN1(acosh, acosh, UNCHANGED)
DEFN_ARRAY_FUN1(acospi, acospi, UNCHANGED)
DEFN_ARRAY_FUN1(asin, asin, UNCHANGED)
DEFN_ARRAY_FUN1(asinh, asinh, UNCHANGED)
DEFN_ARRAY_FUN1(asinpi, asinpi, UNCHANGED)
DEFN_ARRAY_FUN1(atan, atan, UNCHANGED)
DEFN_ARRAY_FUN1(atanh, atanh, UNCHANGED)
DEFN_ARRAY_FUN1(atanpi, atanpi, UNCHANGED)
DEFN_ARRAY_FUN1(cbrt, cbrt, UNCHANGED)
DEFN_ARRAY_FUN1(ceil, ceil, UNCHANGED)
DEFN_ARRAY_FUN1(clz, clz, UNCHANGED)
DEFN_ARRAY_FUN1(cos, cos, UNCHANGED)
DEFN_ARRAY_FUN1(cosh, cosh, UNCHANGED)
DEFN_ARRAY_FUN1(cospi, cospi, UNCHANGED)
DEFN_ARRAY_FUN1(degrees, degrees, UNCHANGED)
DEFN_ARRAY_FUN1(erfc, erfc, UNCHANGED)
DEFN_ARRAY_FUN1(erf, erf, UNCHANGED)
DEFN_ARRAY_FUN1(exp, exp, UNCHANGED)
DEFN_ARRAY_FUN1(exp2, exp2, UNCHANGED)
DEFN_ARRAY_FUN1(exp10, exp10, UNCHANGED)
DEFN_ARRAY_FUN1(expm1, expm1, UNCHANGED)
DEFN_ARRAY_FUN1(fabs, fabs, UNCHANGED)
DEFN_ARRAY_FUN1(floor, floor, UNCHANGED)
DEFN_ARRAY_FUN1(ilogb, ilogb, UNCHANGED)
DEFN_ARRAY_FUN1(isfinite, isfinite, UNCHANGED)
DEFN_ARRAY_FUN1(isinf, isinf, UNCHANGED)
DEFN_ARRAY_FUN1(isnan, isnan, UNCHANGED)
DEFN_ARRAY_FUN1(isnormal, isnormal, UNCHANGED)
DEFN_ARRAY_FUN1(lgamma, lgamma, UNCHANGED)
DEFN_ARRAY_FUN1(log, log, UNCHANGED)
DEFN_ARRAY_FUN1(log2, log2, UNCHANGED)
DEFN_ARRAY_FUN1(log10, log10, UNCHANGED)
DEFN_ARRAY_FUN1(log1p, log1p, UNCHANGED)
DEFN_ARRAY_FUN1(logb, logb, UNCHANGED)
DEFN_ARRAY_FUN1(mean, mean, REDUCED)
DEFN_ARRAY_FUN1(nan, nan, UNCHANGED)
DEFN_ARRAY_FUN1(operator-, operatorUNARYMINUS, UNCHANGED)
DEFN_ARRAY_FUN1(operator~, operatorCOMPLEMENT, UNCHANGED)
DEFN_ARRAY_FUN1(operator!, operatorNOT, UNCHANGED)
DEFN_ARRAY_FUN1(radians, radians, UNCHANGED)
DEFN_ARRAY_FUN1(rint, rint, UNCHANGED)
DEFN_ARRAY_FUN1(round, round, UNCHANGED)
DEFN_ARRAY_FUN1(rsqrt, rsqrt, UNCHANGED)
DEFN_ARRAY_FUN1(sign, sign, UNCHANGED)
DEFN_ARRAY_FUN1(signbit, signbit, UNCHANGED)
DEFN_ARRAY_FUN1(sin, sin, UNCHANGED)
DEFN_ARRAY_FUN1(sinh, sinh, UNCHANGED)
DEFN_ARRAY_FUN1(sinpi, sinpi, UNCHANGED)
DEFN_ARRAY_FUN1(sqrt, sqrt, UNCHANGED)
DEFN_ARRAY_FUN1(sum, sum, REDUCED)
DEFN_ARRAY_FUN1(tan, tan, UNCHANGED)
DEFN_ARRAY_FUN1(tanh, tanh, UNCHANGED)
DEFN_ARRAY_FUN1(tanpi, tanpi, UNCHANGED)
DEFN_ARRAY_FUN1(tgamma, tgamma, UNCHANGED)
DEFN_ARRAY_FUN1(transpose, transpose, TRANSPOSED)
DEFN_ARRAY_FUN1(trunc, trunc, UNCHANGED)

#define DEFN_ARRAY_FUN2(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN2(dot_product, dot_product, REDUCED)
DEFN_ARRAY_FUN2(gather1_floor, gather1_floor, FIRST2)
DEFN_ARRAY_FUN2(isordered, isordered, FIRST2)
DEFN_ARRAY_FUN2(isunordered, isunordered, FIRST2)
DEFN_ARRAY_FUN2(matmul, matmul, MATMUL)

#define DEFN_ARRAY_FUN3(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B, const ArrayExp& C) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, C, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN3(clamp, clamp, FIRST3)
DEFN_ARRAY_FUN3(fma, fma, FIRST3)
DEFN_ARRAY_FUN3(gather2_floor, gather2_floor, FIRST3)
DEFN_ARRAY_FUN3(mad, mad, FIRST3)
DEFN_ARRAY_FUN3(mad24, mad24, FIRST3)
DEFN_ARRAY_FUN3(mad_hi, mad_hi, FIRST3)
DEFN_ARRAY_FUN3(mad_sat, mad_sat, FIRST3)
DEFN_ARRAY_FUN3(mix, mix, FIRST3)
DEFN_ARRAY_FUN3(smoothstep, smoothstep, FIRST3)

////////////////////////////////////////
// special function for general matmul

MatmulFun::MatmulFun(const ArrayExp& a,
                     const ArrayExp& b,
                     const ArrayExp& c,
                     const double alpha,
                     const double beta)
    : _alpha(alpha),
      _beta(beta),
      _funCode(ByteCodes::matmulG),
      _a(a),
      _b(b),
      _c(c) { }

void MatmulFun::accept(Stak<BC>& v) const
{
    v.push(_beta);
    v.push(_alpha);
    _c.accept(v);
    _b.accept(v);
    _a.accept(v);

    v.push(_funCode);
}

void MatmulFun::accept(stack< ArrayDim >& v) const
{
    _c.accept(v);
}

MatmulFun matmul (const ArrayExp& A, const ArrayExp& B, const ArrayExp& C)
{
    return MatmulFun(A, B, C, 1.0f, 1.0f);
}

MatmulFun matmul (const ArrayExp& A,
                  const ArrayExp& B,
                  const ArrayExp& C,
                  const double alpha,
                  const double beta)
{
    return MatmulFun(A, B, C, alpha, beta);
}

////////////////////////////////////////
// special function for index creation

IndexFun::IndexFun(const uint32_t f,
                   const size_t indexDim,
                   const size_t W,
                   const size_t H)
    : _funCode(f),
      _pushH(ByteCodes::index2_f32 == f || ByteCodes::index2_f64 == f),
      _indexDim(indexDim),
      _W(W),
      _H(H) { }

void IndexFun::accept(Stak<BC>& v) const
{
    if (_pushH)
        v.push(static_cast<double>(_H));

    v.push(static_cast<double>(_W));
    v.push(static_cast<double>(_indexDim));

    v.push(_funCode);
}

void IndexFun::accept(stack< ArrayDim >& v) const
{
    v.push(ArrayDim(_W, _H));
}

#define DEFN_INDEX_FUN(FP) \
IndexFun index_ ## FP (const size_t indexDim, const size_t W) \
{ \
    return IndexFun(ByteCodes::index1_ ## FP, indexDim, W, 1); \
} \
IndexFun index_ ## FP (const size_t indexDim, const size_t W, const size_t H) \
{ \
    return IndexFun(ByteCodes::index2_ ## FP, indexDim, W, H); \
}

DEFN_INDEX_FUN(u32)
DEFN_INDEX_FUN(i32)
DEFN_INDEX_FUN(f32)
DEFN_INDEX_FUN(f64)

////////////////////////////////////////
// (two argument) function value

DuoFun::DuoFun(const uint32_t f,
               const ArrayExp& a,
               const ArrayExp& b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(b) { }

DuoFun::DuoFun(const uint32_t f,
               const ArrayExp& a,
               const uint32_t b)
    : _lit32u(ArrayLitu32(b)), // b
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(_lit32u) { }

DuoFun::DuoFun(const uint32_t f,
               const ArrayExp& a,
               const int32_t b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(b)), // b
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(_lit32i) { }

DuoFun::DuoFun(const uint32_t f,
               const ArrayExp& a,
               const float b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(b)), // b
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(_lit32f) { }

DuoFun::DuoFun(const uint32_t f,
               const ArrayExp& a,
               const double b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(b)), // b
      _funCode(f),
      _a(a),
      _b(_lit64f) { }

DuoFun::DuoFun(const uint32_t f,
               const uint32_t a,
               const ArrayExp& b)
    : _lit32u(ArrayLitu32(a)), // a
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(_lit32u),
      _b(b) { }

DuoFun::DuoFun(const uint32_t f,
               const int32_t a,
               const ArrayExp& b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(a)), // a
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(_lit32i),
      _b(b) { }

DuoFun::DuoFun(const uint32_t f,
               const float a,
               const ArrayExp& b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(a)), // a
      _lit64f(ArrayLitf64(0)),
      _funCode(f),
      _a(_lit32f),
      _b(b) { }

DuoFun::DuoFun(const uint32_t f,
               const double a,
               const ArrayExp& b)
    : _lit32u(ArrayLitu32(0)),
      _lit32i(ArrayLiti32(0)),
      _lit32f(ArrayLitf32(0)),
      _lit64f(ArrayLitf64(a)), // a
      _funCode(f),
      _a(_lit64f),
      _b(b) { }

void DuoFun::accept(Stak<BC>& v) const
{
    _b.accept(v);
    _a.accept(v);

    v.push(_funCode);
}

void DuoFun::accept(stack< ArrayDim >& v) const
{
    _b.accept(v);
    _a.accept(v);

    const ArrayDim a = v.top();
    v.pop();
    const ArrayDim b = v.top();
    v.pop();

    const size_t maxW = a.width > b.width ? a.width : b.width;
    const size_t maxH = a.height > b.height ? a.height : b.height;
    const size_t maxSlots = (a.slots > b.slots ? a.slots : b.slots);

    v.push( ArrayDim(maxW, maxH, maxSlots) );
}

////////////////////////////////////////
// ternary predicate

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const ArrayExp& a,
               const ArrayExp& b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(a),
      _b(b) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const ArrayExp& a,
               const uint32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(b)), // b
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(a),
      _b(_lit32uB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const ArrayExp& a,
               const int32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(b)), // b
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(a),
      _b(_lit32iB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const ArrayExp& a,
               const float b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(b)), // b
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(a),
      _b(_lit32fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const ArrayExp& a,
               const double b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(b)), // b
      _funCode(f),
      _pred(p),
      _a(a),
      _b(_lit64fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const uint32_t a,
               const ArrayExp& b)
    : _lit32uA(ArrayLitu32(a)), // a
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32uA),
      _b(b) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const int32_t a,
               const ArrayExp& b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(a)), // a
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32iA),
      _b(b) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const float a,
               const ArrayExp& b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(a)), // a
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32fA),
      _b(b) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const double a,
               const ArrayExp& b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(a)), // a
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit64fA),
      _b(b) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const uint32_t a,
               const uint32_t b)
    : _lit32uA(ArrayLitu32(a)), // a
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(b)), // b
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32uA),
      _b(_lit32uB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const uint32_t a,
               const int32_t b)
    : _lit32uA(ArrayLitu32(a)), // a
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(b)), // b
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32uA),
      _b(_lit32iB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const uint32_t a,
               const float b)
    : _lit32uA(ArrayLitu32(a)), // a
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(b)), // b
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32uA),
      _b(_lit32fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const uint32_t a,
               const double b)
    : _lit32uA(ArrayLitu32(a)), // a
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(b)), // b
      _funCode(f),
      _pred(p),
      _a(_lit32uA),
      _b(_lit64fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const int32_t a,
               const uint32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(a)), // a
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(b)), // b
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32iA),
      _b(_lit32uB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const int32_t a,
               const int32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(a)), // a
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(b)), // b
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32iA),
      _b(_lit32iB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const int32_t a,
               const float b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(a)), // a
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(b)), // b
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32iA),
      _b(_lit32fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const int32_t a,
               const double b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(a)), // a
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(b)), // b
      _funCode(f),
      _pred(p),
      _a(_lit32iA),
      _b(_lit64fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const float a,
               const uint32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(a)), // a
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(b)), // b
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32fA),
      _b(_lit32uB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const float a,
               const int32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(a)), // a
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(b)), // b
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32fA),
      _b(_lit32iB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const float a,
               const float b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(a)), // a
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(b)), // b
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit32fA),
      _b(_lit32fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const float a,
               const double b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(a)), // a
      _lit64fA(ArrayLitf64(0)),
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(b)), // b
      _funCode(f),
      _pred(p),
      _a(_lit32fA),
      _b(_lit64fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const double a,
               const uint32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(a)), // a
      _lit32uB(ArrayLitu32(b)), // b
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit64fA),
      _b(_lit32uB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const double a,
               const int32_t b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(a)), // a
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(b)), // b
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit64fA),
      _b(_lit32iB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const double a,
               const float b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(a)), // a
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(b)), // b
      _lit64fB(ArrayLitf64(0)),
      _funCode(f),
      _pred(p),
      _a(_lit64fA),
      _b(_lit32fB) { }

TriFun::TriFun(const uint32_t f,
               const ArrayExp& p,
               const double a,
               const double b)
    : _lit32uA(ArrayLitu32(0)),
      _lit32iA(ArrayLiti32(0)),
      _lit32fA(ArrayLitf32(0)),
      _lit64fA(ArrayLitf64(a)), // a
      _lit32uB(ArrayLitu32(0)),
      _lit32iB(ArrayLiti32(0)),
      _lit32fB(ArrayLitf32(0)),
      _lit64fB(ArrayLitf64(b)), // b
      _funCode(f),
      _pred(p),
      _a(_lit64fA),
      _b(_lit64fB) { }

void TriFun::accept(Stak<BC>& v) const
{
    _b.accept(v);
    _a.accept(v);
    _pred.accept(v);

    v.push(_funCode);
}

void TriFun::accept(stack< ArrayDim >& v) const
{
    _b.accept(v);
    _a.accept(v);

    const ArrayDim a = v.top();
    v.pop();
    const ArrayDim b = v.top();
    v.pop();

    v.push(ArrayDim( (a.width > b.width ? a.width : b.width),
                     (a.height > b.height ? a.height : b.height),
                     (a.slots > b.slots ? a.slots : b.slots) ));
}

TriFun cond(const ArrayExp& pred, const ArrayExp& A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const ArrayExp& A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const uint32_t A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const uint32_t A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const int32_t A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const float A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const double A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

////////////////////////////////////////
// binary operations

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

////////////////////////////////////////
// random number generators

#define DEFN_RNG(FP, X) \
RNG ## FP ::RNG ## FP (const RNG ## FP ## Variant rngVariant) \
    : _rngVariant(rngVariant) { }

DEFN_RNG(u32, uint32_t)
DEFN_RNG(i32, int32_t)
DEFN_RNG(f32, float)
DEFN_RNG(f64, double)

////////////////////////////////////////
// array variables

#define DEFN_ARRAY(FP, X, PREC) \
\
void ARR(FP)::initClient(void) { \
    if (! _client) { \
        _client = Scheduler::singleton().client(); \
        _variable = _client->variable(); \
        _version = 1; \
        _client->constructor(_variable, _nut); } } \
\
ARR(FP)::ARR(FP) (const X lit) \
    : _client(Scheduler::singleton().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut), \
      _W(1), \
      _H(1), \
      _slots(1) { \
    _refs.checkout(_nut); \
    _client->constructor(_variable, _nut); \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    v.push(static_cast<double>(lit)); \
    v.push(ByteCodes::scalar_ ## FP); } \
\
ARR(FP)::ARR(FP) (void) \
    : _client(NULL), \
      _variable(-1), \
      _version(-1), \
      _refs(), \
      _nut(new SingleNut), \
      _W(-1), \
      _H(-1), \
      _slots(-1) { _refs.checkout(_nut); } \
\
ARR(FP)::ARR(FP) (const ARR(FP)& other) \
    : _client(Scheduler::singleton().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut), \
      _W(other._W), \
      _H(other._H), \
      _slots(other._slots) { \
    _refs.checkout(_nut); \
    _client->constructor(_variable, _nut); \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    v.push(BC(other._variable, other._version - 1)); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::ARR(FP) (const ArrayExp& rhs) \
    : _client(Scheduler::singleton().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut) { \
    stack< ArrayDim > dimStack; \
    rhs.accept(dimStack); \
    _W = dimStack.top().width; \
    _H = dimStack.top().height; \
    _slots = dimStack.top().slots; \
    _refs.checkout(_nut); \
    _client->constructor(_variable, _nut); \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    rhs.accept(v); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::~ARR(FP) (void) { \
    if (_client) _client->destructor(_variable); } \
\
ARR(FP)& ARR(FP)::operator= (const ARR(FP)& other) { \
    initClient(); \
    _W = other._W; \
    _H = other._H; \
    _slots = other._slots; \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    v.push(BC(other._variable, other._version - 1)); \
    v.push(ByteCodes::convert_ ## FP); \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator= (const ArrayExp& rhs) { \
    initClient(); \
    stack< ArrayDim > dimStack; \
    rhs.accept(dimStack); \
    _W = dimStack.top().width; \
    _H = dimStack.top().height; \
    _slots = dimStack.top().slots; \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    rhs.accept(v); \
    _version++; \
    v.push(ByteCodes::convert_ ## FP); \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator+= (const X rhs) { \
    *this = *this + rhs; \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator+= (const ARR(FP)& rhs) { \
    *this = *this + rhs; \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator+= (const ArrayExp& rhs) { \
    *this = *this + rhs; \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator-= (const X rhs) { \
    *this = *this - rhs; \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator-= (const ARR(FP)& rhs) { \
    *this = *this - rhs; \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator-= (const ArrayExp& rhs) { \
    *this = *this - rhs; \
    return *this; } \
\
void ARR(FP)::accept(Stak<BC>& v) const { \
    v.push(BC(_variable, _version - 1)); } \
\
void ARR(FP)::accept(stack< ArrayDim >& v) const { \
    v.push(ArrayDim(_W, _H, _slots)); } \
\
X ARR(FP)::read_scalar(void) { \
    const size_t standardVectorLength = PrecType::vecLength(PREC); \
    FrontMem* m = _client->memalloc(_variable, \
                                    standardVectorLength, \
                                    _H, \
                                    PREC); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read_scalar_ ## FP); \
    _client->schedule(); \
    if (2 == PrecType::vecLength(PREC)) \
        return static_cast<X*>(m->ptrMem(PREC))[0] \
             + static_cast<X*>(m->ptrMem(PREC))[1]; \
    else \
        return static_cast<X*>(m->ptrMem(PREC))[0] \
             + static_cast<X*>(m->ptrMem(PREC))[1] \
             + static_cast<X*>(m->ptrMem(PREC))[2] \
             + static_cast<X*>(m->ptrMem(PREC))[3]; } \
\
vector< X > ARR(FP)::read_scalar(const size_t slots) { \
    const size_t standardVectorLength = PrecType::vecLength(PREC); \
    FrontMem* m = _client->memalloc(_variable, \
                                    standardVectorLength, \
                                    _H, \
                                    PREC, \
                                    slots); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read_scalar_ ## FP); \
    _client->schedule(); \
    vector< X > rvec; \
    for (vector< FrontMem* >::const_iterator \
         it = m->slotMem().begin(); \
         it != m->slotMem().end(); \
         it++) \
        rvec.push_back( \
            2 == PrecType::vecLength(PREC) \
                ? static_cast<X*>((*it)->ptrMem(PREC))[0] \
                      + static_cast<X*>((*it)->ptrMem(PREC))[1] \
                : static_cast<X*>((*it)->ptrMem(PREC))[0] \
                      + static_cast<X*>((*it)->ptrMem(PREC))[1] \
                      + static_cast<X*>((*it)->ptrMem(PREC))[2] \
                      + static_cast<X*>((*it)->ptrMem(PREC))[3] ); \
    return rvec; } \
\
void ARR(FP)::read1(X* cpu, \
                    const size_t n, \
                    const size_t stride) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    PREC); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    _client->schedule(); \
    if (PrecType::sizeOf(PREC) == stride) \
        memcpy(cpu, m->ptrMem(PREC), n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem(PREC)) + i, \
                   PrecType::sizeOf(PREC)); \
    return; } \
\
void ARR(FP)::read1(const vector< X* >& cpu, \
                    const size_t n, \
                    const size_t stride) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    PREC, \
                                    cpu.size()); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    _client->schedule(); \
    for (size_t i = 0; i < cpu.size(); i++) \
        if (PrecType::sizeOf(PREC) == stride) \
            memcpy(cpu[i], m->slotMem()[i]->ptrMem(PREC), n); \
        else \
            for (size_t j = 0; j < n; j+=stride) \
                memcpy(reinterpret_cast<char*>(cpu[i]) + j, \
                       static_cast<char*>(m->slotMem()[i] \
                                           ->ptrMem(PREC)) + j, \
                       PrecType::sizeOf(PREC)); \
    return; } \
\
void ARR(FP)::read2(X* cpu, \
                    const size_t n, \
                    const size_t stride, \
                    const size_t pad) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    PREC); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    _client->schedule(); \
    if (PrecType::sizeOf(PREC) == stride) \
        memcpy(cpu, static_cast<char*>(m->ptrMem(PREC)) + pad, n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem(PREC)) + i + pad, \
                   PrecType::sizeOf(PREC)); \
    return; } \
\
void ARR(FP)::read2(const vector< X* >& cpu, \
                    const size_t n, \
                    const size_t stride, \
                    const size_t pad) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    PREC, \
                                    cpu.size()); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    _client->schedule(); \
    for (size_t i = 0; i < cpu.size(); i++) \
        if (PrecType::sizeOf(PREC) == stride) \
            memcpy(cpu[i], static_cast<char*>(m->slotMem()[i] \
                                               ->ptrMem(PREC)) + pad, n); \
        else \
            for (size_t j = 0; j < n; j+=stride) \
                memcpy(reinterpret_cast<char*>(cpu[i]) + j, \
                       static_cast<char*>(m->slotMem()[i] \
                                           ->ptrMem(PREC)) + j + pad, \
                       PrecType::sizeOf(PREC)); \
    return; } \
\
ARR(FP) ARR(FP)::index(const size_t i, const size_t W) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(W)); \
    v.push(static_cast<double>(i)); \
    v.push(ByteCodes::index1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::index(const size_t i, const size_t W, const size_t H) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = H; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(static_cast<double>(i)); \
    v.push(ByteCodes::index2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, X* cpu) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
    a._slots = 1; \
    FrontMem* m = a._client->memalloc(a._variable, W, 1, cpu); \
    void* mx = a._client->frontMem(m); \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, \
                       const vector< X* >& cpu) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
    a._slots = cpu.size(); \
    FrontMem* m = a._client->memalloc(a._variable, W, 1, cpu); \
    void* mx = a._client->frontMem(m); \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, X* cpu) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = H; \
    a._slots = 1; \
    FrontMem* m = a._client->memalloc(a._variable, W, H, cpu); \
    void* mx = a._client->frontMem(m); \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, \
                       const vector< X* >& cpu) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = H; \
    a._slots = cpu.size(); \
    FrontMem* m = a._client->memalloc(a._variable, W, H, cpu); \
    void* mx = a._client->frontMem(m); \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::zeros(const size_t W) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::zeros1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::zeros(const size_t W, const size_t H) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = H; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::zeros2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::ones(const size_t W) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::ones1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::ones(const size_t W, const size_t H) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = H; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::ones2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::rng_uniform_make(RNG ## FP & generator, \
                                  const size_t length, \
                                  const size_t step, \
                                  const X minLimit, \
                                  const X maxLimit) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = length; \
    a._H = 1; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(maxLimit)); \
    v.push(static_cast<double>(minLimit)); \
    v.push(static_cast<double>(step)); \
    v.push(static_cast<double>(length)); \
    v.push(static_cast<double>(generator._rngVariant)); \
    v.push(ByteCodes::rng_uniform_make_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::rng_normal_make(RNG ## FP & generator, \
                                 const size_t length) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = length; \
    a._H = 1; \
    a._slots = 1; \
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(length)); \
    v.push(static_cast<double>(generator._rngVariant)); \
    v.push(ByteCodes::rng_normal_make_ ## FP); \
    return a; } \
\
ARR(FP) make1(const size_t W, X* cpu) { \
    return ARR(FP)::make1(W, cpu); } \
\
ARR(FP) make1(const size_t W, \
              const vector< X* >& cpu) { \
    return ARR(FP)::make1(W, cpu); } \
\
ARR(FP) make2(const size_t W, const size_t H, X* cpu) { \
    return ARR(FP)::make2(W, H, cpu); } \
\
ARR(FP) make2(const size_t W, const size_t H, \
              const vector< X* >& cpu) { \
    return ARR(FP)::make2(W, H, cpu); } \
\
ARR(FP) zeros_ ## FP (const size_t W) { \
    return ARR(FP)::zeros(W); } \
\
ARR(FP) zeros_ ## FP (const size_t W, const size_t H) { \
    return ARR(FP)::zeros(W, H); } \
\
ARR(FP) ones_ ## FP (const size_t W) { \
    return ARR(FP)::ones(W); } \
\
ARR(FP) ones_ ## FP (const size_t W, const size_t H) { \
    return ARR(FP)::ones(W, H); } \
\
ARR(FP) rng_uniform_make(RNG ## FP & generator, \
                         const size_t length, \
                         const size_t step, \
                         const X minLimit, \
                         const X maxLimit) { \
    return ARR(FP)::rng_uniform_make(generator, \
                                     length, \
                                     step, \
                                     minLimit, \
                                     maxLimit); } \
\
ARR(FP) rng_normal_make(RNG ## FP & generator, \
                        const size_t length) { \
    return ARR(FP)::rng_normal_make(generator, \
                                    length); }

DEFN_ARRAY(u32, uint32_t, PrecType::UInt32)
DEFN_ARRAY(i32, int32_t, PrecType::Int32)
DEFN_ARRAY(f32, float, PrecType::Float)
DEFN_ARRAY(f64, double, PrecType::Double)

}; // namespace chai
