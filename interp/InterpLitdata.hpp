// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_LITDATA_HPP_
#define _CHAI_INTERP_LITDATA_HPP_

#include <cstddef>
#include <stdint.h>

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// ones1_u32, ones1_i64
// ones1_f32, ones1_f64
// ones2_u32, ones2_i64
// ones2_f32, ones2_f64
// zeros1_u32, zeros1_i64
// zeros1_f32, zeros1_f64
// zeros2_u32, zeros2_i64
// zeros2_f32, zeros2_f64

class InterpLitdata : public BaseInterp
{
    const size_t   _prec;

    const size_t   _N;

    const uint32_t _uintValue;
    const int32_t  _intValue;
    const float    _floatValue;
    const double   _doubleValue;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpLitdata(const size_t N, const uint32_t a);
    InterpLitdata(const size_t N, const int32_t a);
    InterpLitdata(const size_t N, const float a);
    InterpLitdata(const size_t N, const double a);
};

}; // namespace chai_internal

#endif
