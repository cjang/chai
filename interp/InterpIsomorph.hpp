// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_ISOMORPH_HPP_
#define _CHAI_INTERP_ISOMORPH_HPP_

#include "BaseInterp.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// abs, exp, negate, sqrt

class InterpIsomorph : public BaseInterp
{
    const SimpleFun1& _fun;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpIsomorph(const SimpleFun1&);
};

}; // namespace chai_internal

#endif
