// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_SCALAR_HPP_
#define _CHAI_INTERP_SCALAR_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// scalar_f32, scalar_f64

class InterpScalar : public BaseInterp
{
    const bool _isDP;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpScalar(const bool isDP);
};

}; // namespace chai_internal

#endif
