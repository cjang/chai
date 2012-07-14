// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// ternary predicate operation

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

}; // namespace chai
