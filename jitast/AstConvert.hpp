// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_CONVERT_HPP_
#define _CHAI_AST_CONVERT_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// convert_u32, convert_i32
// convert_f32, convert_f64

class AstConvert : public BaseAst
{
    const bool _isNOP;

public:
    AstConvert(BaseAst* barg,
               const size_t PREC);

    bool isNOP(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
