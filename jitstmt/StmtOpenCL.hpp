// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_OPENCL_HPP_
#define _CHAI_STMT_OPENCL_HPP_

#include "VisitAst.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// inline OpenCL

class StmtOpenCL : public Stmt
{
    // RHS
    AstOpenCL* _rhsOpenCL;

public:
    StmtOpenCL(AstOpenCL* rhs);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstOpenCL* openclPtr(void) const;
};

}; // namespace chai_internal

#endif
