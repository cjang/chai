// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_LITDATA_HPP_
#define _CHAI_TRANS_LITDATA_HPP_

#include <stdint.h>

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransLitdata : public BaseTrans
{
    const size_t   _N;
    const size_t   _prec;

    const uint32_t _uintValue;
    const int32_t  _intValue;
    const float    _floatValue;
    const double   _doubleValue;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransLitdata(const size_t N, const uint32_t value);
    TransLitdata(const size_t N, const int32_t value);
    TransLitdata(const size_t N, const float value);
    TransLitdata(const size_t N, const double value);
};

}; // namespace chai_internal

#endif
