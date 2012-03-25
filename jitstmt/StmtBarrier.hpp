// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_BARRIER_HPP_
#define _CHAI_STMT_BARRIER_HPP_

#include <set>

#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// thread barrier statement

class StmtBarrier : public Stmt
{
    const std::set< uint32_t >           _traceVars;
    const std::set< const AstVariable* > _splitVars;

    bool checkMatch(const AstVariable*) const;

public:
    StmtBarrier(const std::set< uint32_t >& traceVars,
                const std::set< const AstVariable* >& splitVars);

    ~StmtBarrier(void);

    bool containedIn(const std::set< uint32_t >& traceVars) const;
    bool containedIn(const std::set< uint32_t >& traceVars,
                     const std::set< const AstVariable* >& splitVars) const;
    const std::set< uint32_t >& traceVars(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);
};

}; // namespace chai_internal

#endif
