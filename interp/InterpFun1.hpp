// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_FUN1_HPP_
#define _CHAI_INTERP_FUN1_HPP_

#include "BaseInterp.hpp"
#include "SimpleFun1.hpp"

namespace chai_internal {

////////////////////////////////////////
// one argument array stream functions

class InterpFun1 : public BaseInterp
{
    const SimpleFun1& _fun;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpFun1(const SimpleFun1&);
};

}; // namespace chai_internal

#endif
