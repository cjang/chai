// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_FUN3_HPP_
#define _CHAI_INTERP_FUN3_HPP_

#include "BaseInterp.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// three argument array stream functions

class InterpFun3 : public BaseInterp
{
    const SimpleFun3& _fun;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpFun3(const SimpleFun3&);
};

}; // namespace chai_internal

#endif
