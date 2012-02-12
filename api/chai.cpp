// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>
#include <stdlib.h>
#include <string.h>

#include "ArrayClient.hpp"
#include "ByteCodes.hpp"
#include "chai/chai.h"
#include "FrontMem.hpp"
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
    v.push(_lit); \
    v.push(ByteCodes::scalar_ ## FP); } \
void ArrayLit ## FP ::accept(stack< ArrayDim >& v) const { \
    v.push(ArrayDim(1, 1)); }

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
DEFN_ARRAY_FUN1(operator-, negate, UNCHANGED)
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

IndexFun index_f32(const size_t indexDim, const size_t W)
{
    return IndexFun(ByteCodes::index1_f32, indexDim, W, 1);
}

IndexFun index_f64(const size_t indexDim, const size_t W)
{
    return IndexFun(ByteCodes::index1_f64, indexDim, W, 1);
}

IndexFun index_f32(const size_t indexDim, const size_t W, const size_t H)
{
    return IndexFun(ByteCodes::index2_f32, indexDim, W, H);
}

IndexFun index_f64(const size_t indexDim, const size_t W, const size_t H)
{
    return IndexFun(ByteCodes::index2_f64, indexDim, W, H);
}

////////////////////////////////////////
// special function for literals

BinLitFun::BinLitFun(const uint32_t f,
                     const ArrayExp& a,
                     const ArrayExp& b)
    : _lit32(ArrayLitf32(0)),
      _lit64(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(b) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const ArrayExp& a,
                     const int b)
    : _lit32(ArrayLitf32(b)),
      _lit64(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(_lit32) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const int a,
                     const ArrayExp& b)
    : _lit32(ArrayLitf32(a)),
      _lit64(ArrayLitf64(0)),
      _funCode(f),
      _a(_lit32),
      _b(b) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const ArrayExp& a,
                     const float b)
    : _lit32(ArrayLitf32(b)),
      _lit64(ArrayLitf64(0)),
      _funCode(f),
      _a(a),
      _b(_lit32) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const float a,
                     const ArrayExp& b)
    : _lit32(ArrayLitf32(a)),
      _lit64(ArrayLitf64(0)),
      _funCode(f),
      _a(_lit32),
      _b(b) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const ArrayExp& a,
                     const double b)
    : _lit32(ArrayLitf32(0)),
      _lit64(ArrayLitf64(b)),
      _funCode(f),
      _a(a),
      _b(_lit64) { }

BinLitFun::BinLitFun(const uint32_t f,
                     const double a,
                     const ArrayExp& b)
    : _lit32(ArrayLitf32(0)),
      _lit64(ArrayLitf64(a)),
      _funCode(f),
      _a(_lit64),
      _b(b) { }

void BinLitFun::accept(Stak<BC>& v) const
{
    _b.accept(v);
    _a.accept(v);

    v.push(_funCode);
}

void BinLitFun::accept(stack< ArrayDim >& v) const
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

////////////////////////////////////////
// predicates

PredicateFun::PredicateFun(const uint32_t f,
                           const ArrayExp& a,
                           const ArrayExp& b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const ArrayExp& a,
                           const int b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const int a,
                           const ArrayExp& b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const ArrayExp& a,
                           const float b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const float a,
                           const ArrayExp& b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const ArrayExp& a,
                           const double b)
    : BinLitFun(f, a, b) { }

PredicateFun::PredicateFun(const uint32_t f,
                           const double a,
                           const ArrayExp& b)
    : BinLitFun(f, a, b) { }

void PredicateFun::accept(Stak<BC>& v) const
{
    BinLitFun::accept(v);
}

void PredicateFun::accept(stack< ArrayDim >& v) const
{
    BinLitFun::accept(v);
}

#define DEFN_ARRAY_PRED(NAME, FUNCODE) \
PredicateFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const ArrayExp& A, const int B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const int A, const ArrayExp& B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const ArrayExp& A, const float B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const float A, const ArrayExp& B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const ArrayExp& A, const double B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); } \
PredicateFun NAME (const double A, const ArrayExp& B) { \
    return PredicateFun(ByteCodes:: FUNCODE, A, B); }

DEFN_ARRAY_PRED(operator==, operatorEQ)
DEFN_ARRAY_PRED(operator!=, operatorNE)
DEFN_ARRAY_PRED(operator<=, operatorLE)
DEFN_ARRAY_PRED(operator>=, operatorGE)
DEFN_ARRAY_PRED(operator<, operatorLT)
DEFN_ARRAY_PRED(operator>, operatorGT)

PredicateFun operator&& (const PredicateFun& A, const PredicateFun& B) {
    return PredicateFun(ByteCodes::operatorAND, A, B); }

PredicateFun operator|| (const PredicateFun& A, const PredicateFun& B) {
    return PredicateFun(ByteCodes::operatorOR, A, B ); }

CondFun::CondFun(const PredicateFun& p,
                 const ArrayExp& a,
                 const ArrayExp& b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(a),
      _b(b) { }

CondFun::CondFun(const PredicateFun& p,
                 const ArrayExp& a,
                 const int b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(a),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const int a,
                 const ArrayExp& b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(b) { }

CondFun::CondFun(const PredicateFun& p,
                 const ArrayExp& a,
                 const float b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(a),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const float a,
                 const ArrayExp& b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(b) { }

CondFun::CondFun(const PredicateFun& p,
                 const ArrayExp& a,
                 const double b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(b)),
      _pred(p),
      _a(a),
      _b(_lit64B) { }

CondFun::CondFun(const PredicateFun& p,
                 const double a,
                 const ArrayExp& b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(a)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit64A),
      _b(b) { }

CondFun::CondFun(const PredicateFun& p,
                 const int a,
                 const int b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const int a,
                 const float b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const int a,
                 const double b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(b)),
      _pred(p),
      _a(_lit32A),
      _b(_lit64B) { }

CondFun::CondFun(const PredicateFun& p,
                 const float a,
                 const int b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const float a,
                 const float b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit32A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const float a,
                 const double b)
    : _lit32A(ArrayLitf32(a)),
      _lit64A(ArrayLitf64(0)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(b)),
      _pred(p),
      _a(_lit32A),
      _b(_lit64B) { }

CondFun::CondFun(const PredicateFun& p,
                 const double a,
                 const int b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(a)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit64A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const double a,
                 const float b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(a)),
      _lit32B(ArrayLitf32(b)),
      _lit64B(ArrayLitf64(0)),
      _pred(p),
      _a(_lit64A),
      _b(_lit32B) { }

CondFun::CondFun(const PredicateFun& p,
                 const double a,
                 const double b)
    : _lit32A(ArrayLitf32(0)),
      _lit64A(ArrayLitf64(a)),
      _lit32B(ArrayLitf32(0)),
      _lit64B(ArrayLitf64(b)),
      _pred(p),
      _a(_lit64A),
      _b(_lit64B) { }

void CondFun::accept(Stak<BC>& v) const
{
    _b.accept(v);
    _a.accept(v);
    _pred.accept(v);

    v.push(ByteCodes::cond);
}

void CondFun::accept(stack< ArrayDim >& v) const
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

CondFun cond(const PredicateFun& pred, const ArrayExp& A, const ArrayExp& B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const ArrayExp& A, const int B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const int A, const ArrayExp& B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const ArrayExp& A, const float B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const float A, const ArrayExp& B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const ArrayExp& A, const double B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const double A, const ArrayExp& B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const int A, const int B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const int A, const float B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const int A, const double B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const float A, const int B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const float A, const float B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const float A, const double B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const double A, const int B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const double A, const float B) {
    return CondFun(pred, A, B); }

CondFun cond(const PredicateFun& pred, const double A, const double B) {
    return CondFun(pred, A, B); }

////////////////////////////////////////
// binary operations

#define DEFN_ARRAY_BINOPFUN(NAME, FUNCODE) \
BinLitFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const ArrayExp& A, const int B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const int A, const ArrayExp& B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const ArrayExp& A, const float B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const float A, const ArrayExp& B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const ArrayExp& A, const double B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); } \
BinLitFun NAME (const double A, const ArrayExp& B) { \
    return BinLitFun(ByteCodes:: FUNCODE, A, B); }

DEFN_ARRAY_BINOPFUN(operator+, operatorADD)
DEFN_ARRAY_BINOPFUN(operator-, operatorSUB)
DEFN_ARRAY_BINOPFUN(operator*, operatorMUL)
DEFN_ARRAY_BINOPFUN(operator/, operatorDIV)
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

DEFN_RNG(f32, float)
DEFN_RNG(f64, double)

////////////////////////////////////////
// array variables

#define DEFN_ARRAY(FP, X) \
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
    v.push(lit); \
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
    const size_t standardVectorLength = sizeof(double) == sizeof(X) ? 2 : 4; \
    FrontMem* m = _client->memalloc(_variable, \
                                    standardVectorLength, \
                                    _H, \
                                    sizeof(X)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read_scalar_ ## FP); \
    _client->schedule(); \
    if (sizeof(double) == sizeof(X)) \
        return static_cast<X*>(m->ptrMem(sizeof(X)))[0] \
             + static_cast<X*>(m->ptrMem(sizeof(X)))[1]; \
    else \
        return static_cast<X*>(m->ptrMem(sizeof(X)))[0] \
             + static_cast<X*>(m->ptrMem(sizeof(X)))[1] \
             + static_cast<X*>(m->ptrMem(sizeof(X)))[2] \
             + static_cast<X*>(m->ptrMem(sizeof(X)))[3]; } \
\
vector< X > ARR(FP)::read_scalar(const size_t slots) { \
    const size_t standardVectorLength = sizeof(double) == sizeof(X) ? 2 : 4; \
    FrontMem* m = _client->memalloc(_variable, \
                                    standardVectorLength, \
                                    _H, \
                                    sizeof(X), \
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
            sizeof(double) == sizeof(X) \
                ? static_cast<X*>((*it)->ptrMem(sizeof(X)))[0] \
                      + static_cast<X*>((*it)->ptrMem(sizeof(X)))[1] \
                : static_cast<X*>((*it)->ptrMem(sizeof(X)))[0] \
                      + static_cast<X*>((*it)->ptrMem(sizeof(X)))[1] \
                      + static_cast<X*>((*it)->ptrMem(sizeof(X)))[2] \
                      + static_cast<X*>((*it)->ptrMem(sizeof(X)))[3] ); \
    return rvec; } \
\
void ARR(FP)::read1(X* cpu, \
                    const size_t n, \
                    const size_t stride) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    sizeof(X)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    _client->schedule(); \
    if (sizeof(X) == stride) \
        memcpy(cpu, m->ptrMem(sizeof(X)), n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem(sizeof(X))) + i, \
                   sizeof(X)); \
    return; } \
\
void ARR(FP)::read1(const vector< X* >& cpu, \
                    const size_t n, \
                    const size_t stride) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    sizeof(X), \
                                    cpu.size()); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    _client->schedule(); \
    for (size_t i = 0; i < cpu.size(); i++) \
        if (sizeof(X) == stride) \
            memcpy(cpu[i], m->slotMem()[i]->ptrMem(sizeof(X)), n); \
        else \
            for (size_t j = 0; j < n; j+=stride) \
                memcpy(reinterpret_cast<char*>(cpu[i]) + j, \
                       static_cast<char*>(m->slotMem()[i] \
                                           ->ptrMem(sizeof(X))) + j, \
                       sizeof(X)); \
    return; } \
\
void ARR(FP)::read2(X* cpu, \
                    const size_t n, \
                    const size_t stride, \
                    const size_t pad) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    sizeof(X)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    _client->schedule(); \
    if (sizeof(X) == stride) \
        memcpy(cpu, static_cast<char*>(m->ptrMem(sizeof(X))) + pad, n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem(sizeof(X))) + i + pad, \
                   sizeof(X)); \
    return; } \
\
void ARR(FP)::read2(const vector< X* >& cpu, \
                    const size_t n, \
                    const size_t stride, \
                    const size_t pad) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    sizeof(X), \
                                    cpu.size()); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    _client->schedule(); \
    for (size_t i = 0; i < cpu.size(); i++) \
        if (sizeof(X) == stride) \
            memcpy(cpu[i], static_cast<char*>(m->slotMem()[i] \
                                               ->ptrMem(sizeof(X))) + pad, n); \
        else \
            for (size_t j = 0; j < n; j+=stride) \
                memcpy(reinterpret_cast<char*>(cpu[i]) + j, \
                       static_cast<char*>(m->slotMem()[i] \
                                           ->ptrMem(sizeof(X))) + j + pad, \
                       sizeof(X)); \
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

DEFN_ARRAY(f32, float)
DEFN_ARRAY(f64, double)

}; // namespace chai
