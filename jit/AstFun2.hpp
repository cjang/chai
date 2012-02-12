// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN2_HPP_
#define _CHAI_AST_FUN2_HPP_

#include "BaseAst.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// functions with two arguments

class AstFun2 : public BaseAst
{
    const SimpleFun2& _fun;

    const size_t _leftW;
    const size_t _leftH;
    const size_t _rightW;
    const size_t _rightH;

public:
    AstFun2(const SimpleFun2& fun,
            BaseAst* bargLeft,
            BaseAst* bargRight);

    const SimpleFun2& fun(void) const;

    size_t leftW(void) const;
    size_t leftH(void) const;
    size_t rightW(void) const;
    size_t rightH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
