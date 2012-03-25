// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstDotprod.hpp"
#include "TransDotprod.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransDotprod::sub_eval(void) const
{
    return
        new AstDotprod(_argStack[0],
                       _argStack[1]);
}

TransDotprod::TransDotprod(void)
    : BaseTrans(0, 2) { }

}; // namespace chai_internal
