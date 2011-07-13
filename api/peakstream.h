// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_PEAKSTREAM_H_
#define _CHAI_PEAKSTREAM_H_

#include <ostream>
#include <set>
#include <stack>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "ArrayClient.hpp"
#include "BC.hpp"
#include "RefCnt.hpp"
#include "SingleNut.hpp"
#include "Stak.hpp"

namespace chai {

////////////////////////////////////////
// virtual machine management

void init(void);
void shutdown(void);

////////////////////////////////////////
// array expression interface

struct ArrayExp {
    virtual ~ArrayExp(void);
    virtual void accept(chai_internal::Stak<chai_internal::BC>&) const = 0;
    virtual void accept(std::stack< std::pair< size_t, size_t > >&) const = 0;
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
    void accept(std::stack< std::pair< size_t, size_t > >&) const; \
};

DECL_ARRAY_LIT(f32, float)
DECL_ARRAY_LIT(f64, double)

////////////////////////////////////////
// array functions

class ArrayFun : public ArrayExp
{
public:
    enum DimStack { UNCHANGED, REDUCED, MATMUL, FIRST2, FIRST3 };

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
    void accept(std::stack< std::pair< size_t, size_t > >&) const;
};

ArrayFun abs (CAER);
ArrayFun exp (CAER);
ArrayFun mean (CAER);
ArrayFun sqrt (CAER);
ArrayFun sum (CAER);
ArrayFun operator- (CAER);

ArrayFun dot_product (CAER, CAER);
ArrayFun gather1_floor (CAER, CAER);
ArrayFun matmul (CAER, CAER);

ArrayFun gather2_floor (CAER, CAER, CAER);

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
    void accept(std::stack< std::pair< size_t, size_t > >&) const;
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
    void accept(std::stack< std::pair< size_t, size_t > >&) const;
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
    void accept(std::stack< std::pair< size_t, size_t > >&) const;
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
// binary operations

#define DECL_ARRAY_BINOP(NAME) \
BinLitFun NAME (CAER, CAER); \
BinLitFun NAME (CAER, const int); \
BinLitFun NAME (const int, CAER); \
BinLitFun NAME (CAER, const float); \
BinLitFun NAME (const float, CAER); \
BinLitFun NAME (CAER, const double); \
BinLitFun NAME (const double, CAER);

DECL_ARRAY_BINOP(operator+)
DECL_ARRAY_BINOP(operator-)
DECL_ARRAY_BINOP(operator*)
DECL_ARRAY_BINOP(operator/)
DECL_ARRAY_BINOP(max)
DECL_ARRAY_BINOP(min)

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
    void accept(std::stack< std::pair< size_t, size_t > >&) const; \
    X read_scalar(void); \
    void read1(X* cpu, const size_t n, \
                       const size_t stride = sizeof(X)); \
    void read2(X* cpu, const size_t n, \
                       const size_t stride = sizeof(X), \
                       const size_t pad = 0); \
    static ARR(FP) index(const size_t i, const size_t W); \
    static ARR(FP) index(const size_t i, const size_t W, const size_t H); \
    static ARR(FP) make1(const size_t W, X* cpu); \
    static ARR(FP) make2(const size_t W, const size_t H, X* cpu); \
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
ARR(FP) index_ ## FP (const size_t i, const size_t W); \
ARR(FP) index_ ## FP (const size_t i, const size_t W, const size_t H); \
ARR(FP) make1(const size_t W, X* cpu); \
ARR(FP) make2(const size_t W, const size_t H, X* cpu); \
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
