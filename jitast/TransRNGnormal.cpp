// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "AstRNGnormal.hpp"
#include "TransRNGnormal.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransRNGnormal::sub_eval(void) const
{
    const int variant   = _argScalar[0];
    const uint64_t seed = _argScalar[1];
    const size_t len    = _argScalar[2];

    return
        new AstRNGnormal(variant,
                         seed,
                         len,
                         _precision);
}

TransRNGnormal::TransRNGnormal(const size_t precision)
    : BaseTrans(3, 0),
      _precision(precision) { }

}; // namespace chai_internal
