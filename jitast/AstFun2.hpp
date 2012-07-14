// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN2_HPP_
#define _CHAI_AST_FUN2_HPP_

#include <string>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// functions with two arguments

class AstFun2 : public BaseAst
{
    const std::string _fun;
    const bool        _infix;

public:
    AstFun2(const std::string& fun,
            const bool infix,
            BaseAst* bargLeft,
            BaseAst* bargRight);

    const std::string& fun(void) const;
    bool infix(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
