// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_SCALAR_HPP_
#define _CHAI_INTERP_SCALAR_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// scalar_u32, scalar_i32
// scalar_f32, scalar_f64

class InterpScalar : public BaseInterp
{
    const size_t _precision;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpScalar(const size_t precision);
};

}; // namespace chai_internal

#endif
