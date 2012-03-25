// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MATMUL_MV_HPP_
#define _CHAI_AST_MATMUL_MV_HPP_

#include "AstMatmulBase.hpp"
#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// matmul (matrix * vector)

class AstMatmulMV : public AstMatmulBase
{
public:
    AstMatmulMV(BaseAst* bargLeft,
                BaseAst* bargRight);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
