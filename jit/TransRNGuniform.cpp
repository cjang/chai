// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "AstRNGuniform.hpp"
#include "TransRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransRNGuniform::sub_eval(void) const
{
    //const unsigned int seed = 0;
    const unsigned int seed = random();
    const size_t variant    = _argScalar[0]; // not used
    const size_t len        = _argScalar[1];
    const size_t step       = _argScalar[2];
    const double minlimit   = _argScalar[3];
    const double maxlimit   = _argScalar[4];

    return
        new AstRNGuniform(seed,
                          variant,
                          len,
                          step,
                          minlimit,
                          maxlimit,
                          _isDouble);
}

TransRNGuniform::TransRNGuniform(const bool isDouble)
    : BaseTrans(5, 0),
      _isDouble(isDouble) { }

}; // namespace chai_internal
