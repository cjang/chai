// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_GATHER_HPP_
#define _CHAI_TRANS_GATHER_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransGather : public BaseTrans
{
    const size_t _N;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransGather(const size_t N);
};

}; // namespace chai_internal

#endif
