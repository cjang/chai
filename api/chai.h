// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_H_
#define _CHAI_H_

#include <istream>
#include <ostream>
#include <set>
#include <stack>
#include <stdint.h>
#include <string>
#include <vector>

#include <chai/BC.hpp>
#include <chai/RefCnt.hpp>
#include <chai/Stak.hpp>

namespace chai_internal {
    class ArrayClient;
    class SingleNut;
};

namespace chai {

////////////////////////////////////////
// virtual machine management

void init(std::istream& configSpecStream);
void init(const std::string& configSpecFileName);
void shutdown(void);

////////////////////////////////////////
// array expression interface

struct ArrayDim
{
    const size_t width;
    const size_t height;
    const size_t slots;

    ArrayDim(const size_t w, const size_t h);
    ArrayDim(const size_t w, const size_t h, const size_t s);
};

struct ArrayExp {
    virtual ~ArrayExp(void);
    virtual void accept(chai_internal::Stak<chai_internal::BC>&) const = 0;
    virtual void accept(std::stack< ArrayDim >&) const = 0;
};

#define CAER const ArrayExp&

////////////////////////////////////////
// array literals

#define DECL_ARRAY_LIT(FP, X) \
class ArrayLit ## FP : public ArrayExp { \
    const X _lit; \
public: \
    ArrayLit ## FP (const X); \
    X value(void) const; \
    void accept(chai_internal::Stak<chai_internal::BC>&) const; \
    void accept(std::stack< ArrayDim >&) const; \
};

DECL_ARRAY_LIT(f32, float)
DECL_ARRAY_LIT(f64, double)

////////////////////////////////////////
// array functions

class ArrayFun : public ArrayExp
{
public:
    enum DimStack { UNCHANGED, REDUCED, MATMUL, FIRST2, FIRST3, TRANSPOSED };

private:
    const DimStack               _dimStack;
    const uint32_t               _funCode;
    std::vector<const ArrayExp*> _argList;

public:
    ArrayFun(const uint32_t, CAER, const DimStack);
    ArrayFun(const uint32_t, CAER, CAER, const DimStack);
    ArrayFun(const uint32_t, CAER, CAER, CAER, const DimStack);
    ArrayFun(const ArrayFun&);

    uint32_t code(void) const;
    void arg(const ArrayExp&);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

ArrayFun acos (CAER);
ArrayFun acosh (CAER);
ArrayFun acospi (CAER);
ArrayFun asin (CAER);
ArrayFun asinh (CAER);
ArrayFun asinpi (CAER);
ArrayFun atan (CAER);
ArrayFun atanh (CAER);
ArrayFun atanpi (CAER);
ArrayFun cbrt (CAER);
ArrayFun ceil (CAER);
ArrayFun cos (CAER);
ArrayFun cosh (CAER);
ArrayFun cospi (CAER);
ArrayFun degrees (CAER);
ArrayFun erfc (CAER);
ArrayFun erf (CAER);
ArrayFun exp (CAER);
ArrayFun exp2 (CAER);
ArrayFun exp10 (CAER);
ArrayFun expm1 (CAER);
ArrayFun fabs (CAER);
ArrayFun floor (CAER);
ArrayFun ilogb (CAER);
ArrayFun isfinite (CAER);
ArrayFun isinf (CAER);
ArrayFun isnan (CAER);
ArrayFun isnormal (CAER);
ArrayFun lgamma (CAER);
ArrayFun log (CAER);
ArrayFun log2 (CAER);
ArrayFun log10 (CAER);
ArrayFun log1p (CAER);
ArrayFun logb (CAER);
ArrayFun mean (CAER);
ArrayFun nan (CAER);
ArrayFun operator- (CAER);
ArrayFun radians (CAER);
ArrayFun rint (CAER);
ArrayFun round (CAER);
ArrayFun rsqrt (CAER);
ArrayFun sign (CAER);
ArrayFun signbit (CAER);
ArrayFun sin (CAER);
ArrayFun sinh (CAER);
ArrayFun sinpi (CAER);
ArrayFun sqrt (CAER);
ArrayFun sum (CAER);
ArrayFun tan (CAER);
ArrayFun tanh (CAER);
ArrayFun tanpi (CAER);
ArrayFun tgamma (CAER);
ArrayFun transpose (CAER);
ArrayFun trunc (CAER);

ArrayFun dot_product (CAER, CAER);
ArrayFun gather1_floor (CAER, CAER);
ArrayFun isordered (CAER, CAER);
ArrayFun isunordered (CAER, CAER);
ArrayFun matmul (CAER, CAER);

ArrayFun clamp (CAER, CAER, CAER);
ArrayFun fma (CAER, CAER, CAER);
ArrayFun gather2_floor (CAER, CAER, CAER);
ArrayFun mad (CAER, CAER, CAER);
ArrayFun mix (CAER, CAER, CAER);
ArrayFun smoothstep (CAER, CAER, CAER);

////////////////////////////////////////
// special function for general matmul

class MatmulFun : public ArrayExp
{
    const double    _alpha;
    const double    _beta;
    const uint32_t  _funCode;
    const ArrayExp& _a;
    const ArrayExp& _b;
    const ArrayExp& _c;

public:
    MatmulFun(CAER A,
              CAER B,
              CAER C,
              const double alpha,
              const double beta);

    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

// GEMM is C = alpha * matmul(A, B) + beta * C
MatmulFun matmul (CAER A, CAER B, CAER C); // alpha = beta = 1.0f
MatmulFun matmul (CAER A,
                  CAER B,
                  CAER C,
                  const double alpha,
                  const double beta);

////////////////////////////////////////
// special function for index creation

class IndexFun : public ArrayExp
{
    const uint32_t _funCode;
    const bool     _pushH;

    const size_t   _indexDim; // 0 means vary across width
                              // 1 means vary down height
    const size_t   _W;
    const size_t   _H;
    
public:
    IndexFun(const uint32_t funCode,
             const size_t indexDim,
             const size_t W,
             const size_t H);

    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

IndexFun index_f32(const size_t indexDim, const size_t W);
IndexFun index_f64(const size_t indexDim, const size_t W);
IndexFun index_f32(const size_t indexDim, const size_t W, const size_t H);
IndexFun index_f64(const size_t indexDim, const size_t W, const size_t H);

////////////////////////////////////////
// special function for literals

class BinLitFun : public ArrayExp
{
    const ArrayLitf32 _lit32;
    const ArrayLitf64 _lit64;
    const uint32_t    _funCode;
    const ArrayExp&   _a;
    const ArrayExp&   _b;

public:
    BinLitFun(const uint32_t, CAER, CAER);
    BinLitFun(const uint32_t, CAER, const int);
    BinLitFun(const uint32_t, const int, CAER);
    BinLitFun(const uint32_t, CAER, const float);
    BinLitFun(const uint32_t, const float, CAER);
    BinLitFun(const uint32_t, CAER, const double);
    BinLitFun(const uint32_t, const double, CAER);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

////////////////////////////////////////
// predicates

struct PredicateFun : public BinLitFun
{
    PredicateFun(const uint32_t, CAER, CAER);
    PredicateFun(const uint32_t, CAER, const int);
    PredicateFun(const uint32_t, const int, CAER);
    PredicateFun(const uint32_t, CAER, const float);
    PredicateFun(const uint32_t, const float, CAER);
    PredicateFun(const uint32_t, CAER, const double);
    PredicateFun(const uint32_t, const double, CAER);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

#define DECL_ARRAY_PRED(NAME) \
PredicateFun NAME (CAER, CAER); \
PredicateFun NAME (CAER, const int); \
PredicateFun NAME (const int, CAER); \
PredicateFun NAME (CAER, const float); \
PredicateFun NAME (const float, CAER); \
PredicateFun NAME (CAER, const double); \
PredicateFun NAME (const double, CAER);

DECL_ARRAY_PRED(operator==)
DECL_ARRAY_PRED(operator!=)
DECL_ARRAY_PRED(operator<=)
DECL_ARRAY_PRED(operator>=)
DECL_ARRAY_PRED(operator<)
DECL_ARRAY_PRED(operator>)

PredicateFun operator&& (const PredicateFun&, const PredicateFun&);
PredicateFun operator|| (const PredicateFun&, const PredicateFun&);

class CondFun : public ArrayExp
{
    const ArrayLitf32   _lit32A;
    const ArrayLitf64   _lit64A;
    const ArrayLitf32   _lit32B;
    const ArrayLitf64   _lit64B;
    const PredicateFun& _pred;
    const ArrayExp&     _a;
    const ArrayExp&     _b;

public:
    CondFun(const PredicateFun&, CAER, CAER);
    CondFun(const PredicateFun&, CAER, const int);
    CondFun(const PredicateFun&, const int, CAER);
    CondFun(const PredicateFun&, CAER, const float);
    CondFun(const PredicateFun&, const float, CAER);
    CondFun(const PredicateFun&, CAER, const double);
    CondFun(const PredicateFun&, const double, CAER);
    CondFun(const PredicateFun&, const int, const int);
    CondFun(const PredicateFun&, const int, const float);
    CondFun(const PredicateFun&, const int, const double);
    CondFun(const PredicateFun&, const float, const int);
    CondFun(const PredicateFun&, const float, const float);
    CondFun(const PredicateFun&, const float, const double);
    CondFun(const PredicateFun&, const double, const int);
    CondFun(const PredicateFun&, const double, const float);
    CondFun(const PredicateFun&, const double, const double);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

CondFun cond(const PredicateFun&, CAER, CAER);
CondFun cond(const PredicateFun&, CAER, const int);
CondFun cond(const PredicateFun&, const int, CAER);
CondFun cond(const PredicateFun&, CAER, const float);
CondFun cond(const PredicateFun&, const float, CAER);
CondFun cond(const PredicateFun&, CAER, const double);
CondFun cond(const PredicateFun&, const double, CAER);
CondFun cond(const PredicateFun&, const int, const int);
CondFun cond(const PredicateFun&, const int, const float);
CondFun cond(const PredicateFun&, const int, const double);
CondFun cond(const PredicateFun&, const float, const int);
CondFun cond(const PredicateFun&, const float, const float);
CondFun cond(const PredicateFun&, const float, const double);
CondFun cond(const PredicateFun&, const double, const int);
CondFun cond(const PredicateFun&, const double, const float);
CondFun cond(const PredicateFun&, const double, const double);

////////////////////////////////////////
// binary operations and functions

#define DECL_ARRAY_BINOPFUN(NAME) \
BinLitFun NAME (CAER, CAER); \
BinLitFun NAME (CAER, const int); \
BinLitFun NAME (const int, CAER); \
BinLitFun NAME (CAER, const float); \
BinLitFun NAME (const float, CAER); \
BinLitFun NAME (CAER, const double); \
BinLitFun NAME (const double, CAER);

DECL_ARRAY_BINOPFUN(operator+)
DECL_ARRAY_BINOPFUN(operator-)
DECL_ARRAY_BINOPFUN(operator*)
DECL_ARRAY_BINOPFUN(operator/)
DECL_ARRAY_BINOPFUN(max)
DECL_ARRAY_BINOPFUN(min)
DECL_ARRAY_BINOPFUN(maxmag)
DECL_ARRAY_BINOPFUN(minmag)
DECL_ARRAY_BINOPFUN(nextafter)
DECL_ARRAY_BINOPFUN(pow)
DECL_ARRAY_BINOPFUN(pown)
DECL_ARRAY_BINOPFUN(powr)
DECL_ARRAY_BINOPFUN(remainder)
DECL_ARRAY_BINOPFUN(rootn)
DECL_ARRAY_BINOPFUN(step)
DECL_ARRAY_BINOPFUN(atan2)
DECL_ARRAY_BINOPFUN(atan2pi)
DECL_ARRAY_BINOPFUN(copysign)
DECL_ARRAY_BINOPFUN(fdim)
DECL_ARRAY_BINOPFUN(fmax)
DECL_ARRAY_BINOPFUN(fmin)
DECL_ARRAY_BINOPFUN(fmod)
DECL_ARRAY_BINOPFUN(hypot)
DECL_ARRAY_BINOPFUN(ldexp)
DECL_ARRAY_BINOPFUN(isequal)
DECL_ARRAY_BINOPFUN(isnotequal)
DECL_ARRAY_BINOPFUN(isgreater)
DECL_ARRAY_BINOPFUN(isgreaterequal)
DECL_ARRAY_BINOPFUN(isless)
DECL_ARRAY_BINOPFUN(islessequal)
DECL_ARRAY_BINOPFUN(islessgreater)

////////////////////////////////////////
// random number generators

enum RNGf32Variant {
    SP_RNG_DEFAULT = 0
};

enum RNGf64Variant {
    DP_RNG_DEFAULT = 0
};

#define DECL_RNG(FP, X) \
class Array ## FP; \
class RNG ## FP { \
    friend class Array ## FP; \
    const RNG ## FP ## Variant _rngVariant; \
public: \
    RNG ## FP (const RNG ## FP ## Variant rngVariant); \
};
 
DECL_RNG(f32, float)
DECL_RNG(f64, double)

////////////////////////////////////////
// array variables

#define ARR(FP) Array ## FP

#define DECL_ARRAY(FP, X) \
class ARR(FP) : public ArrayExp { \
    chai_internal::ArrayClient* _client; \
    uint32_t                    _variable; \
    uint32_t                    _version; \
    chai_internal::RefSet       _refs; \
    chai_internal::SingleNut*   _nut; \
    size_t                      _W; \
    size_t                      _H; \
    size_t                      _slots; \
    void initClient(void); \
public: \
    ARR(FP) (const X); \
    ARR(FP) (void); \
    ARR(FP) (const ARR(FP)&); \
    ARR(FP) (const ArrayExp&); \
    ~ARR(FP) (void); \
    ARR(FP)& operator= (const ARR(FP)&); \
    ARR(FP)& operator= (const ArrayExp&); \
    ARR(FP)& operator+= (const X); \
    ARR(FP)& operator+= (const ARR(FP)&); \
    ARR(FP)& operator+= (const ArrayExp&); \
    ARR(FP)& operator-= (const X); \
    ARR(FP)& operator-= (const ARR(FP)&); \
    ARR(FP)& operator-= (const ArrayExp&); \
    void accept(chai_internal::Stak<chai_internal::BC>&) const; \
    void accept(std::stack< ArrayDim >&) const; \
    X read_scalar(void); \
    std::vector< X > read_scalar(const size_t slots); \
    void read1(X* cpu, \
               const size_t n, \
               const size_t stride = sizeof(X)); \
    void read1(const std::vector< X* >& cpu, \
               const size_t n, \
               const size_t stride = sizeof(X)); \
    void read2(X* cpu, \
               const size_t n, \
               const size_t stride = sizeof(X), \
               const size_t pad = 0); \
    void read2(const std::vector< X* >& cpu, \
               const size_t n, \
               const size_t stride = sizeof(X), \
               const size_t pad = 0); \
    static ARR(FP) index(const size_t i, const size_t W); \
    static ARR(FP) index(const size_t i, const size_t W, const size_t H); \
    static ARR(FP) make1(const size_t W, X* cpu); \
    static ARR(FP) make1(const size_t W, \
                         const std::vector< X* >& cpu); \
    static ARR(FP) make2(const size_t W, const size_t H, X* cpu); \
    static ARR(FP) make2(const size_t W, const size_t H, \
                         const std::vector< X* >& cpu); \
    static ARR(FP) zeros(const size_t W); \
    static ARR(FP) zeros(const size_t W, const size_t H); \
    static ARR(FP) ones(const size_t W); \
    static ARR(FP) ones(const size_t W, const size_t H); \
    static ARR(FP) rng_uniform_make(RNG ## FP & generator, \
                                    const size_t length, \
                                    const size_t step, \
                                    const X minlimit, \
                                    const X maxlimit); \
    static ARR(FP) rng_normal_make(RNG ## FP & generator, \
                                   const size_t length); \
}; \
ARR(FP) make1(const size_t W, X* cpu); \
ARR(FP) make1(const size_t W, \
              const std::vector< X* >& cpu); \
ARR(FP) make2(const size_t W, const size_t H, X* cpu); \
ARR(FP) make2(const size_t W, const size_t H, \
              const std::vector< X* >& cpu); \
ARR(FP) zeros_ ## FP (const size_t W); \
ARR(FP) zeros_ ## FP (const size_t W, const size_t H); \
ARR(FP) ones_ ## FP (const size_t W); \
ARR(FP) ones_ ## FP (const size_t W, const size_t H); \
ARR(FP) rng_uniform_make(RNG ## FP & generator, \
                         const size_t length, \
                         const size_t step, \
                         const X minLimit, \
                         const X maxLimit); \
ARR(FP) rng_normal_make(RNG ## FP & generator, \
                        const size_t length);

DECL_ARRAY(f32, float)
DECL_ARRAY(f64, double)

}; // namespace chai

#endif
