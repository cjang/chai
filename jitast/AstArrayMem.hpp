// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_ARRAY_MEM_HPP_
#define _CHAI_AST_ARRAY_MEM_HPP_

#include <stdint.h>

#include "BaseAst.hpp"
#include "BackMem.hpp"
#include "FrontMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// array memory

class AstArrayMem : public BaseAst
{
    const uint32_t _variable;
    const uint32_t _version;

    std::vector< FrontMem* > _frontMem;
    BackMem*                 _backMem;

public:
    AstArrayMem(const std::vector< FrontMem* >& frontMem,
                BackMem* backMem,
                const uint32_t variable,
                const uint32_t version);

    const std::vector< FrontMem* >& frontMem(void) const;
    BackMem* backMem(void) const;

    uint32_t variable(void) const;
    uint32_t version(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
