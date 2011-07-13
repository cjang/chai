// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_RNG_RAND_HPP_
#define _CHAI_X_STMT_RNG_RAND_HPP_

#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// random number generation

class XStmtRNGrand : public XStmt
{
    // RHS
    AstRNGnormal*  _rhsRNGnormal;
    AstRNGuniform* _rhsRNGuniform;

public:
    XStmtRNGrand(AstVariable* lhs, AstRNGnormal* rhs);
    XStmtRNGrand(AstVariable* lhs, AstRNGuniform* rhs);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    AstRNGnormal* normalPtr(void) const;
    AstRNGuniform* uniformPtr(void) const;
};

}; // namespace chai_internal

#endif
