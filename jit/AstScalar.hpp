// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_SCALAR_HPP_
#define _CHAI_AST_SCALAR_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// scalar_f32, scalar_f64

class AstScalar : public BaseAst
{
    const float _floatValue;
    const double _doubleValue;

public:
    AstScalar(const float floatValue);
    AstScalar(const double doubleValue);

    float floatValue(void) const;
    double doubleValue(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
