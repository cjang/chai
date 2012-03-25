// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulVV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulMM.hpp"
#include "TransMatmulG.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransMatmulG::sub_eval(void) const
{
    const size_t leftH = _argStack[0]->H();
    const size_t rightH = _argStack[1]->H();

    if ( (1 == leftH) && (1 == rightH) )
    {
        // vector * vector
        AstMatmulVV* m = new AstMatmulVV(_argStack[0], _argStack[1]);
        m->setGEMM(_argStack[2], _argScalar[0], _argScalar[1]);
        return m;
    }
    else if (1 == leftH)
    {
        // vector * matrix
        AstMatmulVM* m = new AstMatmulVM(_argStack[0], _argStack[1]);
        m->setGEMM(_argStack[2], _argScalar[0], _argScalar[1]);
        return m;
    }
    else if (1 == rightH)
    {
        // matrix * vector
        AstMatmulMV* m = new AstMatmulMV(_argStack[0], _argStack[1]);
        m->setGEMM(_argStack[2], _argScalar[0], _argScalar[1]);
        return m;
    }
    else
    {
        // matrix * matrix
        AstMatmulMM* m = new AstMatmulMM(_argStack[0], _argStack[1]);
        m->setGEMM(_argStack[2], _argScalar[0], _argScalar[1]);
        return m;
    }
}

TransMatmulG::TransMatmulG(void)
    : BaseTrans(2, 3) { }

}; // namespace chai_internal
