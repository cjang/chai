// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstTranspose.hpp"
#include "TransTranspose.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransTranspose::sub_eval(void) const
{
    return
        new AstTranspose(_argStack[0]);
}

TransTranspose::TransTranspose(void)
    : BaseTrans(0, 1) { }

}; // namespace chai_internal
