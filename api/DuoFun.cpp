// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// two argument operation

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

}; // namespace chai
