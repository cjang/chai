// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_NOP_HPP_
#define _CHAI_INTERP_NOP_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// language extension interpreter object

class InterpNOP : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpNOP(void);

    BaseInterp* clone(void) const;
};

}; // namespace chai_internal

#endif
