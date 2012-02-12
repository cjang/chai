// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANS_READOUT_HPP_
#define _CHAI_TRANS_READOUT_HPP_

#include "BaseTrans.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

class TransReadout : public BaseTrans
{
    const size_t _readVarDim;

protected:
    BaseAst* sub_eval(void) const;

public:
    TransReadout(const size_t readVarDim); // 0 is read_scalar,
                                           // 1 is read1
                                           // 2 is read2
};

}; // namespace chai_internal

#endif
