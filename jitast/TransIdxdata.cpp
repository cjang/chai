// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
                           _precision);
    }
    else
    {
        return
            new AstIdxdata(_argScalar[0],
                           _argScalar[1],
                           _argScalar[2],
                           _precision);
    }
}

TransIdxdata::TransIdxdata(const size_t N,
                           const size_t precision)
    : BaseTrans(N+1, 0),
      _N(N),
      _precision(precision) { }

}; // namespace chai_internal
