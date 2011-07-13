// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_LITDATA_HPP_
#define _CHAI_TRANS_LITDATA_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransLitdata : public BaseTrans
{
    const size_t _N;
    const bool   _isDouble;
    const float  _floatValue;
    const double _doubleValue;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransLitdata(const size_t N,
                 const float value);

    TransLitdata(const size_t N,
                 const double value);
};

}; // namespace chai_internal

#endif
