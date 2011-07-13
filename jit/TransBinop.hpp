// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_BINOP_HPP_
#define _CHAI_TRANS_BINOP_HPP_

#include "SimpleFuns.hpp"
#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransBinop : public BaseTrans
{
    const SimpleFun2& _fun;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransBinop(const SimpleFun2& fun);
};

}; // namespace chai_internal

#endif
