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
    const size_t W      = _argScalar[2];
    const size_t H      = _argScalar[3];
    const size_t slots  = _argScalar[4];

    return
        new AstRNGnormal(variant,
                         seed,
                         _prec,
                         W,
                         H,
                         slots);
}

TransRNGnormal::TransRNGnormal(const size_t PREC)
    : BaseTrans(5, 0),
      _prec(PREC) { }

}; // namespace chai_internal
