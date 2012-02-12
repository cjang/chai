// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_MATMUL_HPP_
#define _CHAI_INTERP_MATMUL_HPP_

#include "InterpMatmulBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// simple matmul

class InterpMatmul : public InterpMatmulBase
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpMatmul(void);
};

}; // namespace chai_internal

#endif
