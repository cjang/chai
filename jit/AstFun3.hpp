// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN3_HPP_
#define _CHAI_AST_FUN3_HPP_

#include "BaseAst.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// functions with three arguments

class AstFun3 : public BaseAst
{
    const SimpleFun3& _fun;

    const size_t _leftW;
    const size_t _leftH;
    const size_t _middleW;
    const size_t _middleH;
    const size_t _rightW;
    const size_t _rightH;

public:
    AstFun3(const SimpleFun3& fun,
            BaseAst* bargLeft,
            BaseAst* bargMiddle,
            BaseAst* bargRight);

    const SimpleFun3& fun(void) const;

    size_t leftW(void) const;
    size_t leftH(void) const;
    size_t middleW(void) const;
    size_t middleH(void) const;
    size_t rightW(void) const;
    size_t rightH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
