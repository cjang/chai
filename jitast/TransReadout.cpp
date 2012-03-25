// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstReadout.hpp"
#include "TransReadout.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransReadout::sub_eval(void) const
{
    return
        new AstReadout(_argMem[0],
                       _backMem[0],
                       _argStack[0],
                       _readVarDim); // 0 is read_scalar
                                     // 1 is read1
                                     // 2 is read2
}

TransReadout::TransReadout(const size_t readVarDim)
    : BaseTrans(1, 1),
      _readVarDim(readVarDim) { }

}; // namespace chai_internal
