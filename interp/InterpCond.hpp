// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_COND_HPP_
#define _CHAI_INTERP_COND_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// cond

class InterpCond : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpCond(void);
};

}; // namespace chai_internal

#endif
