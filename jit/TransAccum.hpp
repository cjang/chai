// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_ACCUM_HPP_
#define _CHAI_TRANS_ACCUM_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransAccum : public BaseTrans
{
    const bool _takeAvg;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransAccum(const bool takeAvg);
};

}; // namespace chai_internal

#endif
