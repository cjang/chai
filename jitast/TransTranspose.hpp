// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_TRANSPOSE_HPP_
#define _CHAI_TRANS_TRANSPOSE_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransTranspose : public BaseTrans
{
protected:
    BaseAst* sub_eval(void) const;

public:
    TransTranspose(void);
};

}; // namespace chai_internal

#endif
