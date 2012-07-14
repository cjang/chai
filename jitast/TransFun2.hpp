// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_FUN2_HPP_
#define _CHAI_TRANS_FUN2_HPP_

#include <string>

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransFun2 : public BaseTrans
{
    const std::string _fun;
    const bool        _infix;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransFun2(const std::string& fun,
              const bool infix);
};

}; // namespace chai_internal

#endif
