// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "AstRNGnormal.hpp"
#include "TransRNGnormal.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransRNGnormal::sub_eval(void) const
{
    //const unsigned int seed = 0;
    const unsigned int seed = random();
    const size_t variant    = _argScalar[0]; // not used
    const size_t len        = _argScalar[1];

    return
        new AstRNGnormal(seed,
                         variant,
                         len,
                         _isDouble);
}

TransRNGnormal::TransRNGnormal(const bool isDouble)
    : BaseTrans(2, 0),
      _isDouble(isDouble) { }

}; // namespace chai_internal
