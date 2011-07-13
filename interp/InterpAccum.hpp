// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_ACCUM_HPP_
#define _CHAI_INTERP_ACCUM_HPP_

#include "BaseInterp.hpp"

namespace chai_internal {

////////////////////////////////////////
// mean, sum

class InterpAccum : public BaseInterp
{
    const bool _takeAvg;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpAccum(const bool takeAvg);
};

}; // namespace chai_internal

#endif
