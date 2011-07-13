// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_LITDATA_HPP_
#define _CHAI_AST_LITDATA_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

class AstLitdata : public BaseAst
{
    const float  _floatValue;
    const double _doubleValue;

public:
    AstLitdata(const float floatValue,
               const size_t W);

    AstLitdata(const double doubleValue,
               const size_t W);

    AstLitdata(const float floatValue,
               const size_t W,
               const size_t H);

    AstLitdata(const double doubleValue,
               const size_t W,
               const size_t H);

    float floatValue(void) const;
    double doubleValue(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
