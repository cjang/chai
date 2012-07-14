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
                        _prec,
                        _argScalar[0],
                        _argScalar[1],
                        _argScalar[2]);
}

TransMakedata::TransMakedata(const size_t PREC)
    : BaseTrans(4, 0),
      _prec(PREC) { }

}; // namespace chai_internal
