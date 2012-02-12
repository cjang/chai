// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstCond.hpp"
#include "TransCond.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransCond::sub_eval(void) const
{
    return
        new AstCond(_argStack[0],
                    _argStack[1],
                    _argStack[2]);
}

TransCond::TransCond(void)
    : BaseTrans(0, 3) { }

}; // namespace chai_internal
