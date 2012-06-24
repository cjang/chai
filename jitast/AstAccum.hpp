// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_ACCUM_HPP_
#define _CHAI_AST_ACCUM_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// mean, sum

class AstAccum : public BaseAst
{
    const bool _takeAvg;

    const size_t _argW;
    const size_t _argH;

public:
    AstAccum(BaseAst* barg,
             const bool takeAvg);

    bool takeAvg(void) const;

    size_t argW(void) const;
    size_t argH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
