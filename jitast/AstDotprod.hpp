// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_DOTPROD_HPP_
#define _CHAI_AST_DOTPROD_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// dot_product

class AstDotprod : public BaseAst
{
    const size_t _insideW;
    const size_t _insideH;

public:
    AstDotprod(BaseAst* bargLeft,
               BaseAst* bargRight);

    size_t insideW(void) const;
    size_t insideH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
