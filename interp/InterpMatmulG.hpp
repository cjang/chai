// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_MATMUL_G_HPP_
#define _CHAI_INTERP_MATMUL_G_HPP_

#include "InterpMatmulBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// general matmul

class InterpMatmulG : public InterpMatmulBase
{
protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpMatmulG(void);
};

}; // namespace chai_internal

#endif
