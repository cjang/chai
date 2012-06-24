// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_OPENCL_HPP_
#define _CHAI_TRANS_OPENCL_HPP_

#include <cstddef>

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransOpenCL : public BaseTrans
{
protected:
    BaseAst* sub_eval(void) const;

public:
    TransOpenCL(void);
};

}; // namespace chai_internal

#endif
