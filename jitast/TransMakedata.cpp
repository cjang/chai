// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMakedata.hpp"
#include "TransMakedata.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransMakedata::sub_eval(void) const
{
    return
        new AstMakedata(_argMem[0],
                        _backMem[0],
                        _argScalar[0],
                        _argScalar[1],
                        _precision);
}

TransMakedata::TransMakedata(const size_t precision)
    : BaseTrans(3, 0),
      _precision(precision) { }

}; // namespace chai_internal
