// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

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

}; // namespace chai
