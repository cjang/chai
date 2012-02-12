// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MATMUL_VM_HPP_
#define _CHAI_AST_MATMUL_VM_HPP_

#include "AstMatmulBase.hpp"
#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// matmul (vector * matrix)

class AstMatmulVM : public AstMatmulBase
{
public:
    AstMatmulVM(BaseAst* bargLeft,
                BaseAst* bargRight);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
