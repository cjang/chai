// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MATMUL_VM_HPP_
#define _CHAI_AST_MATMUL_VM_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// matmul (vector * matrix)

class AstMatmulVM : public BaseAst
{
    const size_t _leftW;
    const size_t _leftH;
    const size_t _rightW;
    const size_t _rightH;

public:
    AstMatmulVM(BaseAst* bargLeft,
                BaseAst* bargRight);

    size_t leftW(void) const;
    size_t leftH(void) const;
    size_t rightW(void) const;
    size_t rightH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
