// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_MATMUL_G_HPP_
#define _CHAI_TRANS_MATMUL_G_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransMatmulG : public BaseTrans
{
protected:
    BaseAst* sub_eval(void) const;

public:
    TransMatmulG(void);
};

}; // namespace chai_internal

#endif
