// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_SCALAR_HPP_
#define _CHAI_AST_SCALAR_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// scalar_u32, scalar_i32
// scalar_f32, scalar_f64

class AstScalar : public BaseAst
{
    const uint32_t _uintValue;
    const int32_t  _intValue;
    const float    _floatValue;
    const double   _doubleValue;

public:
    AstScalar(const uint32_t uintValue);
    AstScalar(const int32_t intValue);
    AstScalar(const float floatValue);
    AstScalar(const double doubleValue);

    uint32_t uintValue(void) const;
    int32_t intValue(void) const;
    float floatValue(void) const;
    double doubleValue(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
