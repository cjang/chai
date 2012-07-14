// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_MAKEDATA_HPP_
#define _CHAI_TRANS_MAKEDATA_HPP_

#include <cstddef>

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransMakedata : public BaseTrans
{
    const size_t _prec;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransMakedata(const size_t PREC);
};

}; // namespace chai_internal

#endif
