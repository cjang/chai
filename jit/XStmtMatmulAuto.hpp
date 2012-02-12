// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_MATMUL_AUTO_HPP_
#define _CHAI_X_STMT_MATMUL_AUTO_HPP_

#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstVariable.hpp"
#include "XStmtMatmulBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned matrix multiply kernel
// request to load cache
// (all cases except outer product)

class XStmtMatmulAuto : public XStmtMatmulBase
{
public:
    XStmtMatmulAuto(AstVariable*, const AstMatmulMM*);
    XStmtMatmulAuto(AstVariable*, const AstMatmulMV*);
    XStmtMatmulAuto(AstVariable*, const AstMatmulVM*);

    void accept(VisitXStmt&);
};

}; // namespace chai_internal

#endif
