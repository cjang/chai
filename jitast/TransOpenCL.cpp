// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstOpenCL.hpp"
#include "TransOpenCL.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransOpenCL::sub_eval(void) const
{
    return static_cast< AstOpenCL* >( _argMem[0][0]->dataPtr() );
}

TransOpenCL::TransOpenCL(void)
    : BaseTrans(1, 0) { }

}; // namespace chai_internal
