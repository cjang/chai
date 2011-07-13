// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstLitdata.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

AstLitdata::AstLitdata(const float floatValue,
                       const size_t W)
    : BaseAst(W, 1, false),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const double doubleValue,
                       const size_t W)
    : BaseAst(W, 1, true),
      _floatValue(0),
      _doubleValue(doubleValue) { }

AstLitdata::AstLitdata(const float floatValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, false),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const double doubleValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, true),
      _floatValue(0),
      _doubleValue(doubleValue) { }

float AstLitdata::floatValue(void) const
{
    return _floatValue;
}

double AstLitdata::doubleValue(void) const
{
    return _doubleValue;
}

void AstLitdata::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
