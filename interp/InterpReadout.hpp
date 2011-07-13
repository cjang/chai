// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_READOUT_HPP_
#define _CHAI_INTERP_READOUT_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// read_scalar, read1, read2

class InterpReadout : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpReadout(void);
};

}; // namespace chai_internal

#endif
