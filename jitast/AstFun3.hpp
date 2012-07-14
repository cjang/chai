// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN3_HPP_
#define _CHAI_AST_FUN3_HPP_

#include <string>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// functions with three arguments

class AstFun3 : public BaseAst
{
    const std::string _fun;

public:
    AstFun3(const std::string& fun,
            BaseAst* bargLeft,
            BaseAst* bargMiddle,
            BaseAst* bargRight);

    const std::string& fun(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
