// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_MATMUL_AUTO_HPP_
#define _CHAI_STMT_MATMUL_AUTO_HPP_

#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstVariable.hpp"
#include "StmtMatmulBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned matrix multiply kernel
// request to load cache
// (all cases except outer product)

class StmtMatmulAuto : public StmtMatmulBase
{
public:
    StmtMatmulAuto(AstVariable*, const AstMatmulMM*);
    StmtMatmulAuto(AstVariable*, const AstMatmulMV*);
    StmtMatmulAuto(AstVariable*, const AstMatmulVM*);

    void accept(VisitStmt&);
};

}; // namespace chai_internal

#endif
