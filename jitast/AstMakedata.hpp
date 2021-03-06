// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MAKEDATA_HPP_
#define _CHAI_AST_MAKEDATA_HPP_

#include "BaseAst.hpp"
#include "BackMem.hpp"
#include "FrontMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// make1_u32, make1_i32, make2_u32, make2_i32
// make1_f32, make1_f64, make2_f32, make2_f64

class AstMakedata : public BaseAst
{
    std::vector< FrontMem* > _frontMem;
    BackMem*                 _backMem;

public:
    AstMakedata(const std::vector< FrontMem* >& frontMem,
                BackMem* backMem,
                const size_t PREC,
                const size_t W,
                const size_t H,
                const size_t slots);

    const std::vector< FrontMem* >& frontMem(void) const;
    BackMem* backMem(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
