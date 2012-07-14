// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_IDXDATA_HPP_
#define _CHAI_AST_IDXDATA_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// index1_u32, index1_i32, index2_u32, index2_i32
// index1_f32, index1_f64, index2_f32, index2_f64

class AstIdxdata : public BaseAst
{
    const size_t _index;

public:
    AstIdxdata(const size_t index,
               const size_t PREC,
               const size_t W);

    AstIdxdata(const size_t index,
               const size_t PREC,
               const size_t W,
               const size_t H);

    size_t index(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
