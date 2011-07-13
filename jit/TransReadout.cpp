// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
                       _argStack[0]);
}

TransReadout::TransReadout(const bool isDouble)
    : BaseTrans(1, 1),
      _isDouble(isDouble) { }

}; // namespace chai_internal
