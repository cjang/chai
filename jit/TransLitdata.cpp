// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstLitdata.hpp"
#include "TransLitdata.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransLitdata::sub_eval(void) const
{
    if (1 == _N)
    {
        if (_isDouble)
        {
            return
                new AstLitdata(_doubleValue,
                               _argScalar[0]);
        }
        else
        {
            return
                new AstLitdata(_floatValue,
                               _argScalar[0]);
        }
    }
    else
    {
        if (_isDouble)
        {
            return
                new AstLitdata(_doubleValue,
                               _argScalar[0],
                               _argScalar[1]);
        }
        else
        {
            return
                new AstLitdata(_floatValue,
                               _argScalar[0],
                               _argScalar[1]);
        }
    }
}

TransLitdata::TransLitdata(const size_t N,
                           const float floatValue)
    : BaseTrans(N, 0),
      _N(N),
      _isDouble(false),
      _floatValue(floatValue),
      _doubleValue(0) { }

TransLitdata::TransLitdata(const size_t N,
                           const double doubleValue)
    : BaseTrans(N, 0),
      _N(N),
      _isDouble(true),
      _floatValue(0),
      _doubleValue(doubleValue) { }

}; // namespace chai_internal
