// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_MAKEDATA_HPP_
#define _CHAI_INTERP_MAKEDATA_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// make1_f32, make1_f64, make2_f32, make2_f64

class InterpMakedata : public BaseInterp
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpMakedata(void);
};

}; // namespace chai_internal

#endif
