// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_CONVERT_HPP_
#define _CHAI_AST_CONVERT_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// convert_f32, convert_f64

class AstConvert : public BaseAst
{
    const bool _isNOP;

public:
    AstConvert(BaseAst* barg,
               const bool isDouble);

    bool isNOP(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
