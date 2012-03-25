// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstConvert.hpp"
#include "TransConvert.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransConvert::sub_eval(void) const
{
    return
        new AstConvert(_argStack[0],
                       _precision);
}

TransConvert::TransConvert(const size_t precision)
    : BaseTrans(0, 1),
      _precision(precision) { }

}; // namespace chai_internal
