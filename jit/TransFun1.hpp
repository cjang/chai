// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_FUN1_HPP_
#define _CHAI_TRANS_FUN1_HPP_

#include "SimpleFuns.hpp"
#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransFun1 : public BaseTrans
{
    const SimpleFun1& _fun;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransFun1(const SimpleFun1& fun);
};

}; // namespace chai_internal

#endif