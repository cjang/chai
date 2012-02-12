// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_FUN2_HPP_
#define _CHAI_TRANS_FUN2_HPP_

#include "SimpleFuns.hpp"
#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransFun2 : public BaseTrans
{
    const SimpleFun2& _fun;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransFun2(const SimpleFun2& fun);
};

}; // namespace chai_internal

#endif
