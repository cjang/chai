// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_FUN1_HPP_
#define _CHAI_AST_FUN1_HPP_

#include <string>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// single argument functions

class AstFun1 : public BaseAst
{
    const std::string _fun;

public:
    AstFun1(const std::string& fun,
            BaseAst* barg);

    const std::string& fun(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
