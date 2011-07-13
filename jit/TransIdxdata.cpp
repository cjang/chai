// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstIdxdata.hpp"
#include "TransIdxdata.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransIdxdata::sub_eval(void) const
{
    if (1 == _N)
    {
        return
            new AstIdxdata(_argScalar[0],
                           _argScalar[1],
                           _isDouble);
    }
    else
    {
        return
            new AstIdxdata(_argScalar[0],
                           _argScalar[1],
                           _argScalar[2],
                           _isDouble);
    }
}

TransIdxdata::TransIdxdata(const size_t N,
                           const bool isDouble)
    : BaseTrans(N+1, 0),
      _N(N),
      _isDouble(isDouble) { }

}; // namespace chai_internal
