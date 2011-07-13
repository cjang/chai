// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_DOTPROD_HPP_
#define _CHAI_INTERP_DOTPROD_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// dot_product

class InterpDotprod : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpDotprod(void);
};

}; // namespace chai_internal

#endif
