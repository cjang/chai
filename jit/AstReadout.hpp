// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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

public:
    AstReadout(const std::vector< FrontMem* >& frontMem,
               BackMem* backMem,
               BaseAst* barg);

    const std::vector< FrontMem* >& frontMem(void) const;
    BackMem* backMem(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
