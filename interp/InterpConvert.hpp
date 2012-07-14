// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_CONVERT_HPP_
#define _CHAI_INTERP_CONVERT_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// convert_f32
// convert_f64
// convert_i32
// convert_u32

class InterpConvert : public BaseInterp
{
    const size_t _prec;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpConvert(const size_t PREC);
};

}; // namespace chai_internal

#endif
