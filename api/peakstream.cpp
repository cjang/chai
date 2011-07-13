// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>
#include <string.h>

#include "ByteCodes.hpp"
#include "FrontMem.hpp"
#include "peakstream.h"
#include "Scheduler.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// virtual machine management

void init(void) { Scheduler::obj().init(); }
void shutdown(void) { Scheduler::obj().shutdown(); }

////////////////////////////////////////
// array expression interface

ArrayExp::~ArrayExp(void) { }

////////////////////////////////////////
// array literals

#define DEFN_ARRAY_LIT(FP, X) \
ArrayLit ## FP :: ArrayLit ## FP (const X a) : _lit(a) { } \
X ArrayLit ## FP ::value(void) const { return _lit; } \
void ArrayLit ## FP ::accept(Stak<BC>& v) const { \
    v.push(_lit); \
    v.push(ByteCodes::scalar_ ## FP); } \
void ArrayLit ## FP ::accept(stack< pair<size_t, size_t> >& v) const { \
    v.push(pair<size_t, size_t>(1, 1)); }

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

void ArrayFun::accept(stack< pair<size_t, size_t> >& v) const
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
            v.pop();
            v.push(pair<size_t, size_t>(1, 1)); // reduction to scalar
            break;

        case (MATMUL) :
            {
            const pair< size_t, size_t > l = v.top();
            v.pop();
            const pair< size_t, size_t > r = v.top();
            v.pop();
            if (1 == l.second && 1 == r.second)
                v.push(pair<size_t, size_t>(r.first, l.first));  // outer prod
            else if (1 == l.second)
                v.push(pair<size_t, size_t>(r.first, 1));        // vec-matrix
            else if (1 == r.second)
                v.push(pair<size_t, size_t>(l.second, 1));       // matrix-vec
            else
                v.push(pair<size_t, size_t>(r.first, l.second)); // matrix mul
            }
            break;

        case (FIRST2) :
            {
            const pair< size_t, size_t > f = v.top();
            v.pop();
            v.pop();
            v.push(f);
            }
            break;

        case (FIRST3) :
            {
            const pair< size_t, size_t > f = v.top();
            v.pop();
            v.pop();
            v.pop();
            v.push(f);
            }
            break;
    }
}

#define DEFN_ARRAY_FUN1(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN1(abs, abs, UNCHANGED)
DEFN_ARRAY_FUN1(exp, exp, UNCHANGED)
DEFN_ARRAY_FUN1(mean, mean, REDUCED)
DEFN_ARRAY_FUN1(sqrt, sqrt, UNCHANGED)
DEFN_ARRAY_FUN1(sum, sum, REDUCED)
DEFN_ARRAY_FUN1(operator-, negate, UNCHANGED)

#define DEFN_ARRAY_FUN2(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN2(dot_product, dot_product, REDUCED)
DEFN_ARRAY_FUN2(gather1_floor, gather1_floor, FIRST2)
DEFN_ARRAY_FUN2(matmul, matmul, MATMUL)

#define DEFN_ARRAY_FUN3(NAME, FUNCODE, DIMSTACK) \
ArrayFun NAME (const ArrayExp& A, const ArrayExp& B, const ArrayExp& C) { \
    return ArrayFun(ByteCodes:: FUNCODE, A, B, C, ArrayFun:: DIMSTACK); }

DEFN_ARRAY_FUN3(gather2_floor, gather2_floor, FIRST3)

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

void BinLitFun::accept(stack< pair<size_t, size_t> >& v) const
{
    _b.accept(v);
    _a.accept(v);

    const pair< size_t, size_t > a = v.top();
    v.pop();
    const pair< size_t, size_t > b = v.top();
    v.pop();
    v.push(pair<size_t, size_t>(
               (a.first > b.first ? a.first : b.first),
               (a.second > b.second ? a.second : b.second) ));
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

void PredicateFun::accept(stack< pair<size_t, size_t> >& v) const
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

void CondFun::accept(stack< pair<size_t, size_t> >& v) const
{
    _b.accept(v);
    _a.accept(v);

    const pair< size_t, size_t > a = v.top();
    v.pop();
    const pair< size_t, size_t > b = v.top();
    v.pop();
    v.push(pair<size_t, size_t>(
               (a.first > b.first ? a.first : b.first),
               (a.second > b.second ? a.second : b.second) ));
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

#define DEFN_ARRAY_BINOP(NAME, FUNCODE) \
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

DEFN_ARRAY_BINOP(operator+, operatorADD)
DEFN_ARRAY_BINOP(operator-, operatorSUB)
DEFN_ARRAY_BINOP(operator*, operatorMUL)
DEFN_ARRAY_BINOP(operator/, operatorDIV)
DEFN_ARRAY_BINOP(max, max)
DEFN_ARRAY_BINOP(min, min)

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
        _client = Scheduler::obj().client(); \
        _variable = _client->variable(); \
        _version = 1; \
        _client->constructor(_variable, _nut); } } \
\
ARR(FP)::ARR(FP) (const X lit) \
    : _client(Scheduler::obj().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut), \
      _W(1), \
      _H(1) { \
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
      _H(-1) { _refs.checkout(_nut); } \
\
ARR(FP)::ARR(FP) (const ARR(FP)& other) \
    : _client(Scheduler::obj().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut), \
      _W(other._W), \
      _H(other._H) { \
    _refs.checkout(_nut); \
    _client->constructor(_variable, _nut); \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    v.push(BC(other._variable, other._version - 1)); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::ARR(FP) (const ArrayExp& rhs) \
    : _client(Scheduler::obj().client()), \
      _variable(_client->variable()), \
      _version(1), \
      _refs(), \
      _nut(new SingleNut) { \
    stack< pair<size_t, size_t> > dimStack; \
    rhs.accept(dimStack); \
    _W = dimStack.top().first; \
    _H = dimStack.top().second; \
    _refs.checkout(_nut); \
    _client->constructor(_variable, _nut); \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    rhs.accept(v); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::~ARR(FP) (void) { \
    if (_client) _client->destructor(_variable); } \
\
ARR(FP)& ARR(FP)::operator= (const ARR(FP)& rhs) { \
    initClient(); \
    _W = rhs._W; \
    _H = rhs._H; \
    Stak<BC>& v = _client->assignment(_variable, _version++); \
    v.push(BC(rhs._variable, rhs._version - 1)); \
    v.push(ByteCodes::convert_ ## FP); \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator= (const ArrayExp& rhs) { \
    initClient(); \
    stack< pair<size_t, size_t> > dimStack; \
    rhs.accept(dimStack); \
    _W = dimStack.top().first; \
    _H = dimStack.top().second; \
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
void ARR(FP)::accept(stack< pair<size_t, size_t> >& v) const { \
    v.push(pair<size_t, size_t>(_W, _H)); } \
\
X ARR(FP)::read_scalar(void) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    static_cast<X*>(NULL)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    _version++; \
    v.push(ByteCodes::read_scalar_ ## FP); \
    _client->schedule(); \
    return static_cast<X*>(m->ptrMem())[0]; } \
\
void ARR(FP)::read1(X* cpu, const size_t n, \
                            const size_t stride) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    static_cast<X*>(NULL)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    _version++; \
    v.push(ByteCodes::read1_ ## FP); \
    _client->schedule(); \
    if (sizeof(X) == stride) \
        memcpy(cpu, m->ptrMem(), n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem()) + i, \
                   sizeof(X)); \
    return; } \
\
void ARR(FP)::read2(X* cpu, const size_t n, \
                            const size_t stride, \
                            const size_t pad) { \
    FrontMem* m = _client->memalloc(_variable, \
                                    _W, \
                                    _H, \
                                    static_cast<X*>(NULL)); \
    void* mx = _client->frontMem(m); \
    Stak<BC>& v = _client->assignment(_variable, _version); \
    v.push(mx); \
    v.push(BC(_variable, _version - 1)); \
    _version++; \
    v.push(ByteCodes::read2_ ## FP); \
    _client->schedule(); \
    if (sizeof(X) == stride) \
        memcpy(cpu, static_cast<char*>(m->ptrMem()) + pad, n); \
    else \
        for (size_t i = 0; i < n; i+=stride) \
            memcpy(reinterpret_cast<char*>(cpu) + i, \
                   static_cast<char*>(m->ptrMem()) + i + pad, \
                   sizeof(X)); \
    return; } \
\
ARR(FP) ARR(FP)::index(const size_t i, const size_t W) { \
    ARR(FP) a; \
    a.initClient(); \
    a._W = W; \
    a._H = 1; \
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
    Stak<BC>& v = a._client->assignment(a._variable, a._version++); \
    v.push(static_cast<double>(length)); \
    v.push(static_cast<double>(generator._rngVariant)); \
    v.push(ByteCodes::rng_normal_make_ ## FP); \
    return a; } \
\
ARR(FP) index_ ## FP (const size_t i, const size_t W) { \
    return ARR(FP)::index(i, W); } \
\
ARR(FP) index_ ## FP (const size_t i, const size_t W, const size_t H) { \
    return ARR(FP)::index(i, W, H); } \
\
ARR(FP) make1(const size_t W, X* cpu) { \
    return ARR(FP)::make1(W, cpu); } \
\
ARR(FP) make2(const size_t W, const size_t H, X* cpu) { \
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
