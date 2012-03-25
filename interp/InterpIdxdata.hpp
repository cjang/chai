// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_IDXDATA_HPP_
#define _CHAI_INTERP_IDXDATA_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// index1_u32, index1_i32
// index1_f32, index1_f64
// index2_u32, index2_i64
// index2_f32, index2_f64

class InterpIdxdata : public BaseInterp
{
    const size_t _precision;
    const size_t _N;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpIdxdata(const size_t precision, const size_t N);
};

}; // namespace chai_internal

#endif
