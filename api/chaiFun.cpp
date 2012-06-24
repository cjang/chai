// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

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

}; // namespace chai
