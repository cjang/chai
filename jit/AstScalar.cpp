// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstScalar.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// scalar_f32, scalar_f64

AstScalar::AstScalar(const float floatValue)
    : BaseAst(1, 1, false),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstScalar::AstScalar(const double doubleValue)
    : BaseAst(1, 1, true),
      _floatValue(0),
      _doubleValue(doubleValue) { }

float AstScalar::floatValue(void) const
{
    return _floatValue;
}

double AstScalar::doubleValue(void) const
{
    return _doubleValue;
}

void AstScalar::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
