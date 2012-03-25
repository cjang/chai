// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MATMUL_VV_HPP_
#define _CHAI_AST_MATMUL_VV_HPP_

#include "AstMatmulBase.hpp"
#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// matmul (vector * vector)

class AstMatmulVV : public AstMatmulBase
{
public:
    AstMatmulVV(BaseAst* bargLeft,
                BaseAst* bargRight);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
