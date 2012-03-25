// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN1_HPP_
#define _CHAI_AST_FUN1_HPP_

#include "BaseAst.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// single argument functions

class AstFun1 : public BaseAst
{
    const SimpleFun1& _fun;

public:
    AstFun1(const SimpleFun1& fun,
            BaseAst* barg);

    const SimpleFun1& fun(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
