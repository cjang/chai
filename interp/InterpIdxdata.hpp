// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_IDXDATA_HPP_
#define _CHAI_INTERP_IDXDATA_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// index1_f32, index1_f64, index2_f32, index2_f64

class InterpIdxdata : public BaseInterp
{
    const bool   _isDP;
    const size_t _N;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpIdxdata(const bool isDP, const size_t N);
};

}; // namespace chai_internal

#endif
