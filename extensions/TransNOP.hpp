// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_NOP_HPP_
#define _CHAI_TRANS_NOP_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// language extension translator object

class TransNOP : public BaseTrans
{
protected:
    BaseAst* sub_eval(void) const;

public:
    TransNOP(void);

    BaseTrans* clone(void) const;
};

}; // namespace chai_internal

#endif
