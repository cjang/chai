// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_FUN2_HPP_
#define _CHAI_INTERP_FUN2_HPP_

#include "BaseInterp.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// two argument array stream functions

class InterpFun2 : public BaseInterp
{
    const SimpleFun2& _fun;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpFun2(const SimpleFun2&);
};

}; // namespace chai_internal

#endif
