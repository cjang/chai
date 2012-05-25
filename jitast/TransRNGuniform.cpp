// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "AstRNGuniform.hpp"
#include "TransRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransRNGuniform::sub_eval(void) const
{
    const int variant     = _argScalar[0];
    const uint64_t seed   = _argScalar[1];
    const size_t len      = _argScalar[2];
    const size_t step     = _argScalar[3];
    const double minlimit = _argScalar[4];
    const double maxlimit = _argScalar[5];

    return
        new AstRNGuniform(variant,
                          seed,
                          len,
                          step,
                          minlimit,
                          maxlimit,
                          _precision);
}

TransRNGuniform::TransRNGuniform(const size_t precision)
    : BaseTrans(6, 0),
      _precision(precision) { }

}; // namespace chai_internal
