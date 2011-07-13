// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_ID_SPACE_HPP_
#define _CHAI_X_STMT_ID_SPACE_HPP_

#include <set>
#include <vector>

#include "VectorTrace.hpp"
#include "XStmt.hpp"
#include "XStmtTogether.hpp"

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

class XStmtIdSpace : public XStmt
{
    const std::vector< XStmt* > _stmts;

    std::set< AstVariable* > _allVariable;
    std::set< AstVariable* > _lhsVariable;
    std::set< AstVariable* > _rhsVariable;

    const size_t _indexW;
    const size_t _indexH;

    const size_t _numTraces;

public:
    XStmtIdSpace(const XStmtTogether&,
                 const VectorTrace&);
    ~XStmtIdSpace(void);

    const std::set< AstVariable* >& allVariable(void) const;
    const std::set< AstVariable* >& lhsVariable(void) const;
    const std::set< AstVariable* >& rhsVariable(void) const;

    size_t indexW(void) const;
    size_t indexH(void) const;
    size_t numTraces(void) const;

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    const std::vector< XStmt* >& stuffInside(void) const;
};

}; // namespace chai_internal

#endif
