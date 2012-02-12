// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ExtNOP.hpp"
#include "TransNOP.hpp"

namespace chai_internal {

////////////////////////////////////////
// language extension translator object

BaseAst* TransNOP::sub_eval(void) const
{
    return
        new ExtNOP(_argStack[0]);
}

TransNOP::TransNOP(void)
    : BaseTrans(0, 1) { }

BaseTrans* TransNOP::clone(void) const
{
    return new TransNOP;
}

}; // namespace chai_internal
