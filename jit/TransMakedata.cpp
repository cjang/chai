// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
                        _isDouble);
}

TransMakedata::TransMakedata(const bool isDouble)
    : BaseTrans(3, 0),
      _isDouble(isDouble) { }

}; // namespace chai_internal
