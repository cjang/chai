// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_BINOP_HPP_
#define _CHAI_INTERP_BINOP_HPP_

#include "BaseInterp.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// max, min, +, &&, /, ==, >=, >, <=, <, *, !=, ||, -

class InterpBinop : public BaseInterp
{
    const SimpleFun2& _fun;

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpBinop(const SimpleFun2&);
};

}; // namespace chai_internal

#endif
