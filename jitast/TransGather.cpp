// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstGather.hpp"
#include "TransGather.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransGather::sub_eval(void) const
{
    if (1 == _N)
    {
        return
            new AstGather(_argStack[0],
                          _argStack[1]);
    }
    else
    {
        return
            new AstGather(_argStack[0],
                          _argStack[1],
                          _argStack[2]);
    }
}

TransGather::TransGather(const size_t N)
    : BaseTrans(0, N+1),
      _N(N) { }

}; // namespace chai_internal
