// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_GATHER_AUTO_HPP_
#define _CHAI_STMT_GATHER_AUTO_HPP_

#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned gathering uses images when
// possible

class StmtGatherAuto : public Stmt
{
    AstVariable* _dataVariable;

public:
    StmtGatherAuto(AstVariable* dataVariable);

    bool randomness(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    AstVariable* dataPtr(void) const;
};

}; // namespace chai_internal

#endif
