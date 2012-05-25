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

DECL_ARRAY_LIT(u32, uint32_t)
DECL_ARRAY_LIT(i32, int32_t)
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

ArrayFun abs (CAER);
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
ArrayFun clz (CAER);
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
ArrayFun operator~ (CAER);
ArrayFun operator! (CAER);
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
ArrayFun mad24 (CAER, CAER, CAER);
ArrayFun mad_hi (CAER, CAER, CAER);
ArrayFun mad_sat (CAER, CAER, CAER);
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

#define DECL_INDEX_FUN(FP) \
IndexFun index_ ## FP (const size_t indexDim, const size_t W); \
IndexFun index_ ## FP (const size_t indexDim, const size_t W, const size_t H);

DECL_INDEX_FUN(u32)
DECL_INDEX_FUN(i32)
DECL_INDEX_FUN(f32)
DECL_INDEX_FUN(f64)

////////////////////////////////////////
// (two argument) function value

class DuoFun : public ArrayExp
{
    const ArrayLitu32 _lit32u;
    const ArrayLiti32 _lit32i;
    const ArrayLitf32 _lit32f;
    const ArrayLitf64 _lit64f;

    const uint32_t    _funCode;

    const ArrayExp&   _a;
    const ArrayExp&   _b;

public:
    DuoFun(const uint32_t, CAER, CAER);
    DuoFun(const uint32_t, CAER, const uint32_t);
    DuoFun(const uint32_t, CAER, const int32_t);
    DuoFun(const uint32_t, CAER, const float);
    DuoFun(const uint32_t, CAER, const double);
    DuoFun(const uint32_t, const uint32_t, CAER);
    DuoFun(const uint32_t, const int32_t, CAER);
    DuoFun(const uint32_t, const float, CAER);
    DuoFun(const uint32_t, const double, CAER);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

////////////////////////////////////////
// (three argument) function value

class TriFun : public ArrayExp
{
    const ArrayLitu32 _lit32uA;
    const ArrayLiti32 _lit32iA;
    const ArrayLitf32 _lit32fA;
    const ArrayLitf64 _lit64fA;
    const ArrayLitu32 _lit32uB;
    const ArrayLiti32 _lit32iB;
    const ArrayLitf32 _lit32fB;
    const ArrayLitf64 _lit64fB;
    const uint32_t    _funCode;
    const ArrayExp&   _pred;
    const ArrayExp&   _a;
    const ArrayExp&   _b;

public:
    TriFun(const uint32_t, const ArrayExp&, CAER, CAER);
    TriFun(const uint32_t, const ArrayExp&, CAER, const uint32_t);
    TriFun(const uint32_t, const ArrayExp&, CAER, const int32_t);
    TriFun(const uint32_t, const ArrayExp&, CAER, const float);
    TriFun(const uint32_t, const ArrayExp&, CAER, const double);
    TriFun(const uint32_t, const ArrayExp&, const uint32_t, CAER);
    TriFun(const uint32_t, const ArrayExp&, const int32_t, CAER);
    TriFun(const uint32_t, const ArrayExp&, const float, CAER);
    TriFun(const uint32_t, const ArrayExp&, const double, CAER);
    TriFun(const uint32_t, const ArrayExp&, const uint32_t, const uint32_t);
    TriFun(const uint32_t, const ArrayExp&, const uint32_t, const int32_t);
    TriFun(const uint32_t, const ArrayExp&, const uint32_t, const float);
    TriFun(const uint32_t, const ArrayExp&, const uint32_t, const double);
    TriFun(const uint32_t, const ArrayExp&, const int32_t, const uint32_t);
    TriFun(const uint32_t, const ArrayExp&, const int32_t, const int32_t);
    TriFun(const uint32_t, const ArrayExp&, const int32_t, const float);
    TriFun(const uint32_t, const ArrayExp&, const int32_t, const double);
    TriFun(const uint32_t, const ArrayExp&, const float, const uint32_t);
    TriFun(const uint32_t, const ArrayExp&, const float, const int32_t);
    TriFun(const uint32_t, const ArrayExp&, const float, const float);
    TriFun(const uint32_t, const ArrayExp&, const float, const double);
    TriFun(const uint32_t, const ArrayExp&, const double, const uint32_t);
    TriFun(const uint32_t, const ArrayExp&, const double, const int32_t);
    TriFun(const uint32_t, const ArrayExp&, const double, const float);
    TriFun(const uint32_t, const ArrayExp&, const double, const double);
    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

TriFun cond(const ArrayExp&, CAER, CAER);
TriFun cond(const ArrayExp&, CAER, const uint32_t);
TriFun cond(const ArrayExp&, CAER, const int32_t);
TriFun cond(const ArrayExp&, CAER, const float);
TriFun cond(const ArrayExp&, CAER, const double);
TriFun cond(const ArrayExp&, const uint32_t, CAER);
TriFun cond(const ArrayExp&, const int32_t, CAER);
TriFun cond(const ArrayExp&, const float, CAER);
TriFun cond(const ArrayExp&, const double, CAER);
TriFun cond(const ArrayExp&, const uint32_t, const uint32_t);
TriFun cond(const ArrayExp&, const uint32_t, const int32_t);
TriFun cond(const ArrayExp&, const uint32_t, const float);
TriFun cond(const ArrayExp&, const uint32_t, const double);
TriFun cond(const ArrayExp&, const int32_t, const uint32_t);
TriFun cond(const ArrayExp&, const int32_t, const int32_t);
TriFun cond(const ArrayExp&, const int32_t, const float);
TriFun cond(const ArrayExp&, const int32_t, const double);
TriFun cond(const ArrayExp&, const float, const uint32_t);
TriFun cond(const ArrayExp&, const float, const int32_t);
TriFun cond(const ArrayExp&, const float, const float);
TriFun cond(const ArrayExp&, const float, const double);
TriFun cond(const ArrayExp&, const double, const uint32_t);
TriFun cond(const ArrayExp&, const double, const int32_t);
TriFun cond(const ArrayExp&, const double, const float);
TriFun cond(const ArrayExp&, const double, const double);

////////////////////////////////////////
// binary operations and functions

DuoFun operator&& (const ArrayExp&, const ArrayExp&);
DuoFun operator|| (const ArrayExp&, const ArrayExp&);

#define DECL_ARRAY_BINOPFUN(NAME) \
DuoFun NAME (CAER, CAER); \
DuoFun NAME (CAER, const uint32_t); \
DuoFun NAME (const uint32_t, CAER); \
DuoFun NAME (CAER, const int32_t); \
DuoFun NAME (const int32_t, CAER); \
DuoFun NAME (CAER, const float); \
DuoFun NAME (const float, CAER); \
DuoFun NAME (CAER, const double); \
DuoFun NAME (const double, CAER);

DECL_ARRAY_BINOPFUN(operator==)
DECL_ARRAY_BINOPFUN(operator!=)
DECL_ARRAY_BINOPFUN(operator<=)
DECL_ARRAY_BINOPFUN(operator>=)
DECL_ARRAY_BINOPFUN(operator<)
DECL_ARRAY_BINOPFUN(operator>)

DECL_ARRAY_BINOPFUN(abs_diff)
DECL_ARRAY_BINOPFUN(add_sat)
DECL_ARRAY_BINOPFUN(hadd)
DECL_ARRAY_BINOPFUN(mul24)
DECL_ARRAY_BINOPFUN(mul_hi)
DECL_ARRAY_BINOPFUN(rhadd)
DECL_ARRAY_BINOPFUN(rotate)
DECL_ARRAY_BINOPFUN(sub_sat)
DECL_ARRAY_BINOPFUN(operator+)
DECL_ARRAY_BINOPFUN(operator-)
DECL_ARRAY_BINOPFUN(operator*)
DECL_ARRAY_BINOPFUN(operator/)
DECL_ARRAY_BINOPFUN(operator%)
DECL_ARRAY_BINOPFUN(operator<<)
DECL_ARRAY_BINOPFUN(operator>>)
DECL_ARRAY_BINOPFUN(operator&)
DECL_ARRAY_BINOPFUN(operator|)
DECL_ARRAY_BINOPFUN(operator^)
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

enum RNGVariant
{
    RNG_PHILOX = 1230,
    RNG_THREEFRY = 1231,
    RNG_DEFAULT = RNG_PHILOX
};

#define DECL_RNG(FP, X) \
class RNG ## FP { \
    const RNGVariant _rngVariant; \
    X _rngSeed; \
public: \
    RNG ## FP (const RNGVariant, const X); \
    RNGVariant rngVariant(void) const; \
    X rngSeed(void) const; \
    void incSeed(void); \
};

DECL_RNG(u32, uint32_t)
DECL_RNG(i32, uint32_t)
DECL_RNG(f32, uint32_t)
DECL_RNG(f64, uint64_t)

class RngFun : public ArrayExp
{
    const size_t _length;

    chai_internal::Stak<chai_internal::BC> _rngBC;

public:
    // uniform
    RngFun(RNGu32& generator,
           const size_t length,
           const size_t step,
           const uint32_t minLimit,
           const uint32_t maxLimit);

    // uniform
    RngFun(RNGi32& generator,
           const size_t length,
           const size_t step,
           const int32_t minLimit,
           const int32_t maxLimit);

    // uniform
    RngFun(RNGf32& generator,
           const size_t length,
           const size_t step,
           const float minLimit,
           const float maxLimit);

    // uniform
    RngFun(RNGf64& generator,
           const size_t length,
           const size_t step,
           const double minLimit,
           const double maxLimit);

    // normal
    RngFun(RNGu32& generator, const size_t length);
    RngFun(RNGi32& generator, const size_t length);
    RngFun(RNGf32& generator, const size_t length);
    RngFun(RNGf64& generator, const size_t length);

    void accept(chai_internal::Stak<chai_internal::BC>&) const;
    void accept(std::stack< ArrayDim >&) const;
};

RngFun rng_uniform_make(RNGu32& generator,
                        const size_t length,
                        const size_t step);

RngFun rng_uniform_make(RNGi32& generator,
                        const size_t length,
                        const size_t step);

RngFun rng_uniform_make(RNGf32& generator,
                        const size_t length,
                        const size_t step,
                        const float minLimit,
                        const float maxLimit);

RngFun rng_uniform_make(RNGf64& generator,
                        const size_t length,
                        const size_t step,
                        const float minLimit,
                        const float maxLimit);

RngFun rng_normal_make(RNGf32& generator,
                       const size_t length);

RngFun rng_normal_make(RNGf64& generator,
                       const size_t length);

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
ARR(FP) ones_ ## FP (const size_t W, const size_t H);

DECL_ARRAY(u32, uint32_t)
DECL_ARRAY(i32, int32_t)
DECL_ARRAY(f32, float)
DECL_ARRAY(f64, double)

}; // namespace chai

#endif
