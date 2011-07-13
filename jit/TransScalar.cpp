// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstScalar.hpp"
#include "TransScalar.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransScalar::sub_eval(void) const
{
    if (_isDouble)
    {
        return
            new AstScalar(static_cast<double>(_argScalar[0]));
    }
    else
    {
        return
            new AstScalar(static_cast<float>(_argScalar[0]));
    }
}

TransScalar::TransScalar(const bool isDouble)
    : BaseTrans(1, 0),
      _isDouble(isDouble) { }

}; // namespace chai_internal
