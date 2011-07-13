// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_GATHER_HPP_
#define _CHAI_AST_GATHER_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// gather1_floor, gather2_floor

class AstGather : public BaseAst
{
    const size_t _N;
    const size_t _dataW;
    const size_t _dataH;
    const size_t _idxW;
    const size_t _idxH;
    const size_t _idyW;
    const size_t _idyH;

public:
    AstGather(BaseAst* bargData,
              BaseAst* bargX);

    AstGather(BaseAst* bargData,
              BaseAst* bargX,
              BaseAst* bargY);

    size_t N(void) const;
    size_t dataW(void) const;
    size_t dataH(void) const;
    size_t idxW(void) const;
    size_t idxH(void) const;
    size_t idyW(void) const;
    size_t idyH(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
