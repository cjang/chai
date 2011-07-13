// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstConvert.hpp"
#include "TransConvert.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransConvert::sub_eval(void) const
{
    return
        new AstConvert(_argStack[0],
                       _isDouble);
}

TransConvert::TransConvert(const bool isDouble)
    : BaseTrans(0, 1),
      _isDouble(isDouble) { }

}; // namespace chai_internal
