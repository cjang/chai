// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_CONVERT_HPP_
#define _CHAI_INTERP_CONVERT_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// convert_f32, convert_f64

class InterpConvert : public BaseInterp
{
    const bool _isDP;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpConvert(const bool isDP);
};

}; // namespace chai_internal

#endif
