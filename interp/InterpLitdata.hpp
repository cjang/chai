// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_LITDATA_HPP_
#define _CHAI_INTERP_LITDATA_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// ones1_f32, ones1_f64, ones2_f32, ones2_f64
// zeros1_f32, zeros1_f64, zeros2_f32, zeros2_f64

class InterpLitdata : public BaseInterp
{
    const bool   _isDP;
    const float  _floatValue;
    const double _doubleValue;
    const size_t _N;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpLitdata(const float a, const size_t N);
    InterpLitdata(const double a, const size_t N);
};

}; // namespace chai_internal

#endif
