// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstLitdata.hpp"
#include "PrecType.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// ones1_u32, ones1_i32, ones2_u32, ones2_i32
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_u32, zeros1_i32, zeros2_u32, zeros2_i32
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

AstLitdata::AstLitdata(const uint32_t uintValue,
                       const size_t W)
    : BaseAst(W, 1, PrecType::UInt32),
      _uintValue(uintValue),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const int32_t intValue,
                       const size_t W)
    : BaseAst(W, 1, PrecType::Int32),
      _uintValue(0),
      _intValue(intValue),
      _floatValue(0),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const float floatValue,
                       const size_t W)
    : BaseAst(W, 1, PrecType::Float),
      _uintValue(0),
      _intValue(0),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const double doubleValue,
                       const size_t W)
    : BaseAst(W, 1, PrecType::Double),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(doubleValue) { }

AstLitdata::AstLitdata(const uint32_t uintValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, PrecType::UInt32),
      _uintValue(uintValue),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const int32_t intValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, PrecType::Int32),
      _uintValue(0),
      _intValue(intValue),
      _floatValue(0),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const float floatValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, PrecType::Float),
      _uintValue(0),
      _intValue(0),
      _floatValue(floatValue),
      _doubleValue(0) { }

AstLitdata::AstLitdata(const double doubleValue,
                       const size_t W,
                       const size_t H)
    : BaseAst(W, H, PrecType::Double),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(doubleValue) { }

uint32_t AstLitdata::uintValue(void) const
{
    return _uintValue;
}

int32_t AstLitdata::intValue(void) const
{
    return _intValue;
}

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
