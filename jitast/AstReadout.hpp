// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_READOUT_HPP_
#define _CHAI_AST_READOUT_HPP_

#include "BaseAst.hpp"
#include "BackMem.hpp"
#include "FrontMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// read_scalar, read1, read2

class AstReadout : public BaseAst
{
    std::vector< FrontMem* > _frontMem;
    BackMem*                 _backMem;

    const size_t _readVarDim;

public:
    AstReadout(const std::vector< FrontMem* >& frontMem,
               BackMem* backMem,
               BaseAst* barg,
               const size_t readVarDim); // 0 is read_scalar
                                         // 1 is read1
                                         // 2 is read2

    size_t readVarDim(void) const;

    const std::vector< FrontMem* >& frontMem(void) const;
    BackMem* backMem(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
