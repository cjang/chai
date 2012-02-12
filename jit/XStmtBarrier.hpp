// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_BARRIER_HPP_
#define _CHAI_X_STMT_BARRIER_HPP_

#include <set>

#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// thread barrier statement

class XStmtBarrier : public XStmt
{
    const std::set< uint32_t >     _traceVars;
    const std::set< AstVariable* > _splitVars;

    bool checkMatch(AstVariable*) const;

public:
    XStmtBarrier(const std::set< uint32_t >& traceVars,
                 const std::set< AstVariable* >& splitVars);

    ~XStmtBarrier(void);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);
};

}; // namespace chai_internal

#endif
