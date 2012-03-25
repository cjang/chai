// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_LITDATA_HPP_
#define _CHAI_AST_LITDATA_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// ones1_uf32, ones1_i32, ones2_u32, ones2_i32
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_u32, zeros1_i32, zeros2_u32, zeros2_i32
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

class AstLitdata : public BaseAst
{
    const uint32_t _uintValue;
    const int32_t  _intValue;
    const float    _floatValue;
    const double   _doubleValue;

public:
    AstLitdata(const uint32_t uintValue,
               const size_t W);

    AstLitdata(const int32_t intValue,
               const size_t W);

    AstLitdata(const float floatValue,
               const size_t W);

    AstLitdata(const double doubleValue,
               const size_t W);

    AstLitdata(const uint32_t uintValue,
               const size_t W,
               const size_t H);

    AstLitdata(const int32_t intValue,
               const size_t W,
               const size_t H);

    AstLitdata(const float floatValue,
               const size_t W,
               const size_t H);

    AstLitdata(const double doubleValue,
               const size_t W,
               const size_t H);

    uint32_t uintValue(void) const;
    int32_t intValue(void) const;
    float floatValue(void) const;
    double doubleValue(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
