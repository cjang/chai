// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>

#include "AstScalar.hpp"
#include "PrecType.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// scalar_u32, scalar_i32
// scalar_f32, scalar_f64

AstScalar::AstScalar(const uint32_t uintValue)
    : BaseAst(PrecType::UInt32, 1, 1, 1, false),
      _uintValue(uintValue),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0) { }

AstScalar::AstScalar(const int32_t intValue)
    : BaseAst(PrecType::Int32, 1, 1, 1, false),
      _uintValue(0),
      _intValue(intValue),
      _floatValue(0),
      _doubleValue(0) { }

AstScalar::AstScalar(const float floatValue)
    : BaseAst(PrecType::Float, 1, 1, 1, false),
      _uintValue(0),
      _intValue(0),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstScalar::AstScalar(const double doubleValue)
    : BaseAst(PrecType::Double, 1, 1, 1, false),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(doubleValue) { }

uint32_t AstScalar::uintValue(void) const
{
    return _uintValue;
}

int32_t AstScalar::intValue(void) const
{
    return _intValue;
}

float AstScalar::floatValue(void) const
{
    return _floatValue;
}

double AstScalar::doubleValue(void) const
{
    return _doubleValue;
}

int AstScalar::floorValue(void) const
{
    switch (prec())
    {
        case (PrecType::UInt32) : return _uintValue;
        case (PrecType::Int32) : return _intValue;
        case (PrecType::Float) : return floorf(_floatValue);
        case (PrecType::Double) : return floor(_doubleValue);

        // should never happen
        default : return 0;
    }
}

void AstScalar::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
