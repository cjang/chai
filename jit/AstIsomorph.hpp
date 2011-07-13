// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_ISOMORPH_HPP_
#define _CHAI_AST_ISOMORPH_HPP_

#include "BaseAst.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// abs, exp, negate, sqrt

class AstIsomorph : public BaseAst
{
    const SimpleFun1& _fun;

public:
    AstIsomorph(const SimpleFun1& fun,
                BaseAst* barg);

    const SimpleFun1& fun(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
