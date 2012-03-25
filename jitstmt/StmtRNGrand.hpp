// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_RNG_RAND_HPP_
#define _CHAI_STMT_RNG_RAND_HPP_

#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// random number generation

class StmtRNGrand : public Stmt
{
    // RHS
    AstRNGnormal*  _rhsRNGnormal;
    AstRNGuniform* _rhsRNGuniform;

public:
    StmtRNGrand(AstVariable* lhs, AstRNGnormal* rhs);
    StmtRNGrand(AstVariable* lhs, AstRNGuniform* rhs);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstRNGnormal* normalPtr(void) const;
    AstRNGuniform* uniformPtr(void) const;
};

}; // namespace chai_internal

#endif
