// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulVV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulMM.hpp"
#include "TransMatmul.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransMatmul::sub_eval(void) const
{
    const size_t leftH = _argStack[0]->H();
    const size_t rightH = _argStack[1]->H();

    if ( (1 == leftH) && (1 == rightH) )
    {
        // vector * vector
        return
            new AstMatmulVV(_argStack[0],
                            _argStack[1]);
    }
    else if (1 == leftH)
    {
        // vector * matrix
        return
            new AstMatmulVM(_argStack[0],
                            _argStack[1]);
    }
    else if (1 == rightH)
    {
        // matrix * vector
        return
            new AstMatmulMV(_argStack[0],
                            _argStack[1]);
    }
    else
    {
        // matrix * matrix
        return
            new AstMatmulMM(_argStack[0],
                            _argStack[1]);
    }
}

TransMatmul::TransMatmul(void)
    : BaseTrans(0, 2) { }

}; // namespace chai_internal
