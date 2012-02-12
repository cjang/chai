// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_TRANSPOSE_HPP_
#define _CHAI_INTERP_TRANSPOSE_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// transpose

class InterpTranspose : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpTranspose(void);
};

}; // namespace chai_internal

#endif
