// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_COND_HPP_
#define _CHAI_TRANS_COND_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransCond : public BaseTrans
{
protected:
    BaseAst* sub_eval(void) const;

public:
    TransCond(void);
};

}; // namespace chai_internal

#endif
