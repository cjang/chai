// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_COND_HPP_
#define _CHAI_AST_COND_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// cond

class AstCond : public BaseAst
{
    const size_t _predicateW;
    const size_t _predicateH;
    const size_t _trueW;
    const size_t _trueH;
    const size_t _falseW;
    const size_t _falseH;

public:
    AstCond(BaseAst* bargPredicate,
            BaseAst* bargTrue,
            BaseAst* bargFalse);

    size_t predicateW(void) const;
    size_t predicateH(void) const;
    size_t trueW(void) const;
    size_t trueH(void) const;
    size_t falseW(void) const;
    size_t falseH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
