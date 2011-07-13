// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_IDXDATA_HPP_
#define _CHAI_TRANS_IDXDATA_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransIdxdata : public BaseTrans
{
    const size_t _N;
    const bool   _isDouble;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransIdxdata(const size_t N,
                 const bool isDouble);
};

}; // namespace chai_internal

#endif
