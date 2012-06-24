// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
// array variable abstract base class

void ArrayBase::initClient(void)
{
    if (! _client)
    {
        _client = Scheduler::singleton().client();
        _variable = _client->variable();
        _version = 1;
        _client->constructor(_variable, _nut);
    }
}

void ArrayBase::setArrayDim(const size_t W,
                            const size_t H,
                            const size_t slots)
{
    // dimensions set on creation, no resizing allowed
    if (! _slots)
    {
        _W = W;
        _H = H;
        _slots = slots;
    }
}

ArrayBase::ArrayBase(const size_t prectype)
    : _prectype(prectype),
      _client(NULL),
      _variable(-1),
      _version(-1),
      _refs(),
      _nut(new SingleNut),
      _W(0),
      _H(0),
      _slots(0)
{
    _refs.checkout(_nut);
}

ArrayBase::ArrayBase(const size_t prectype,
                     const size_t W,
                     const size_t H,
                     const size_t slots)
    : _prectype(prectype),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(W),
      _H(H),
      _slots(slots)
{
    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::ArrayBase(const ArrayBase& other)
    : _prectype(other._prectype),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(other._W),
      _H(other._H),
      _slots(other._slots)
{
    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::ArrayBase(const size_t prectype,
                     const ArrayExp& rhs)
    : _prectype(prectype),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(0),
      _H(0),
      _slots(0)
{
    stack< ArrayDim > dimStack;
    rhs.accept(dimStack);

    setArrayDim(dimStack.top().width,
                dimStack.top().height,
                dimStack.top().slots);

    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::~ArrayBase(void)
{
    if (_client)
        _client->destructor(_variable);
}

ArrayBase& ArrayBase::assignOther(const ArrayBase& other)
{
    initClient();

    setArrayDim(other._W,
                other._H,
                other._slots);
}

ArrayBase& ArrayBase::assignExp(const ArrayExp& rhs)
{
    initClient();

    stack< ArrayDim > dimStack;
    rhs.accept(dimStack);

    setArrayDim(dimStack.top().width,
                dimStack.top().height,
                dimStack.top().slots);
}

ArrayClient& ArrayBase::client(void) const
{
    return *_client;
}

uint32_t ArrayBase::variable(void) const
{
    return _variable;
}

uint32_t ArrayBase::version(void) const
{
    return _version;
}

uint32_t ArrayBase::versionInc(void)
{
    return _version++;
}

size_t ArrayBase::W(void) const
{
    return _W;
}

size_t ArrayBase::H(void) const
{
    return _H;
}

size_t ArrayBase::slots(void) const
{
    return _slots;
}

ArrayDim ArrayBase::arrayDim(void) const
{
    return ArrayDim(_W, _H, _slots);
}

ArrayClient* ArrayBase::getArrayClient(void) const
{
    return _client;
}

size_t ArrayBase::getArrayPrecType(void) const
{
    return _prectype;
}

uint32_t ArrayBase::getArrayVarNum(void) const
{
    return _variable;
}

////////////////////////////////////////
// array variable derived class

#define DEFN_ARRAY(FP, X, PREC) \
\
ARR(FP)::ARR(FP) (const size_t W, const size_t H, const size_t slots) \
    : ArrayBase(PREC, W, H, slots) { } \
\
ARR(FP)::ARR(FP) (void) \
    : ArrayBase(PREC) { } \
\
ARR(FP)::ARR(FP) (const ARR(FP)& other) \
    : ArrayBase(other) { \
    Stak<BC>& v = client().assignment(variable(), versionInc()); \
    v.push(BC(other.variable(), other.version() - 1)); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::ARR(FP) (const ArrayExp& rhs) \
    : ArrayBase(PREC, rhs) { \
    Stak<BC>& v = client().assignment(variable(), versionInc()); \
    rhs.accept(v); \
    v.push(ByteCodes::convert_ ## FP); } \
\
ARR(FP)::~ARR(FP) (void) { } \
\
ARR(FP)& ARR(FP)::operator= (const ARR(FP)& other) { \
    assignOther(other); \
    Stak<BC>& v = client().assignment(variable(), versionInc()); \
    v.push(BC(other.variable(), other.version() - 1)); \
    v.push(ByteCodes::convert_ ## FP); \
    return *this; } \
\
ARR(FP)& ARR(FP)::operator= (const ArrayExp& rhs) { \
    assignExp(rhs); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    rhs.accept(v); \
    versionInc(); \
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
    v.push(BC(variable(), version() - 1)); } \
\
void ARR(FP)::accept(stack< ArrayDim >& v) const { \
    v.push(arrayDim()); } \
\
X ARR(FP)::read_scalar(void) { \
    const size_t standardVectorLength = PrecType::vecLength(PREC); \
    FrontMem* m = client().memalloc(variable(), \
                                    standardVectorLength, \
                                    H(), \
                                    PREC); \
    void* mx = client().frontMem(m); \
    client().readScalar(variable()); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read_scalar_ ## FP); \
    client().schedule(); \
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
    FrontMem* m = client().memalloc(variable(), \
                                    standardVectorLength, \
                                    H(), \
                                    PREC, \
                                    slots); \
    void* mx = client().frontMem(m); \
    client().readScalar(variable()); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read_scalar_ ## FP); \
    client().schedule(); \
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
    FrontMem* m = client().memalloc(variable(), \
                                    W(), \
                                    H(), \
                                    PREC); \
    void* mx = client().frontMem(m); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    client().schedule(); \
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
    FrontMem* m = client().memalloc(variable(), \
                                    W(), \
                                    H(), \
                                    PREC, \
                                    cpu.size()); \
    void* mx = client().frontMem(m); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read1_ ## FP); \
    client().schedule(); \
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
    FrontMem* m = client().memalloc(variable(), \
                                    W(), \
                                    H(), \
                                    PREC); \
    void* mx = client().frontMem(m); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    client().schedule(); \
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
    FrontMem* m = client().memalloc(variable(), \
                                    W(), \
                                    H(), \
                                    PREC, \
                                    cpu.size()); \
    void* mx = client().frontMem(m); \
    Stak<BC>& v = client().assignment(variable(), version()); \
    v.push(mx); \
    v.push(BC(variable(), version() - 1)); \
    v.push(ByteCodes::read2_ ## FP); \
    client().schedule(); \
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
    ARR(FP) a(W, 1, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(W)); \
    v.push(static_cast<double>(i)); \
    v.push(ByteCodes::index1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::index(const size_t i, const size_t W, const size_t H) { \
    ARR(FP) a(W, H, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(static_cast<double>(i)); \
    v.push(ByteCodes::index2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, X* cpu) { \
    ARR(FP) a(W, 1, 1); \
    FrontMem* m = a.client().memalloc(a.variable(), W, 1, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, \
                       const vector< X* >& cpu) { \
    ARR(FP) a(W, 1, cpu.size()); \
    FrontMem* m = a.client().memalloc(a.variable(), W, 1, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, X* cpu) { \
    ARR(FP) a(W, H, 1); \
    FrontMem* m = a.client().memalloc(a.variable(), W, H, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, \
                       const vector< X* >& cpu) { \
    ARR(FP) a(W, H, cpu.size()); \
    FrontMem* m = a.client().memalloc(a.variable(), W, H, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, X* cpu, \
                       ArrayMemoryOverride vlen) { \
    ARR(FP) a(W, 1, 1); \
    FrontMem* m = a.client().memalloc(a.variable(), W, 1, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    a.client().forceVectorLength(a.variable(), vlen); \
    return a; } \
\
ARR(FP) ARR(FP)::make1(const size_t W, \
                       const vector< X* >& cpu, \
                       ArrayMemoryOverride vlen) { \
    ARR(FP) a(W, 1, cpu.size()); \
    FrontMem* m = a.client().memalloc(a.variable(), W, 1, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(1)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make1_ ## FP); \
    a.client().forceVectorLength(a.variable(), vlen); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, X* cpu, \
                       ArrayMemoryOverride vlen) { \
    ARR(FP) a(W, H, 1); \
    FrontMem* m = a.client().memalloc(a.variable(), W, H, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    a.client().forceVectorLength(a.variable(), vlen); \
    return a; } \
\
ARR(FP) ARR(FP)::make2(const size_t W, const size_t H, \
                       const vector< X* >& cpu, \
                       ArrayMemoryOverride vlen) { \
    ARR(FP) a(W, H, cpu.size()); \
    FrontMem* m = a.client().memalloc(a.variable(), W, H, cpu); \
    void* mx = a.client().frontMem(m); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(mx); \
    v.push(static_cast<double>(H)); /*needed for bytecode hashing*/ \
    v.push(static_cast<double>(W)); /*needed for bytecode hashing*/ \
    v.push(ByteCodes::make2_ ## FP); \
    a.client().forceVectorLength(a.variable(), vlen); \
    return a; } \
\
ARR(FP) ARR(FP)::zeros(const size_t W) { \
    ARR(FP) a(W, 1, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::zeros1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::zeros(const size_t W, const size_t H) { \
    ARR(FP) a(W, H, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::zeros2_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::ones(const size_t W) { \
    ARR(FP) a(W, 1, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::ones1_ ## FP); \
    return a; } \
\
ARR(FP) ARR(FP)::ones(const size_t W, const size_t H) { \
    ARR(FP) a(W, H, 1); \
    Stak<BC>& v = a.client().assignment(a.variable(), a.versionInc()); \
    v.push(static_cast<double>(H)); \
    v.push(static_cast<double>(W)); \
    v.push(ByteCodes::ones2_ ## FP); \
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
ARR(FP) make1(const size_t W, X* cpu, \
              ArrayMemoryOverride vlen) { \
    return ARR(FP)::make1(W, cpu, vlen); } \
\
ARR(FP) make1(const size_t W, \
              const vector< X* >& cpu, \
              ArrayMemoryOverride vlen) { \
    return ARR(FP)::make1(W, cpu, vlen); } \
\
ARR(FP) make2(const size_t W, const size_t H, X* cpu, \
              ArrayMemoryOverride vlen) { \
    return ARR(FP)::make2(W, H, cpu, vlen); } \
\
ARR(FP) make2(const size_t W, const size_t H, \
              const vector< X* >& cpu, \
              ArrayMemoryOverride vlen) { \
    return ARR(FP)::make2(W, H, cpu, vlen); } \
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
    return ARR(FP)::ones(W, H); }

DEFN_ARRAY(u32, uint32_t, PrecType::UInt32)
DEFN_ARRAY(i32, int32_t, PrecType::Int32)
DEFN_ARRAY(f32, float, PrecType::Float)
DEFN_ARRAY(f64, double, PrecType::Double)

}; // namespace chai
