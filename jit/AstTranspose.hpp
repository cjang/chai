// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_TRANSPOSE_HPP_
#define _CHAI_AST_TRANSPOSE_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// transpose

class AstTranspose : public BaseAst
{
public:
    AstTranspose(BaseAst* barg);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
