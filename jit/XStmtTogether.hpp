// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_TOGETHER_HPP_
#define _CHAI_X_STMT_TOGETHER_HPP_

#include <set>
#include <vector>

#include "AstVariable.hpp"
#include "VisitXStmt.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// index space compatibility

class XStmtTogether : public VisitXStmt
{
    std::vector< XStmt* > _togetherList;

    std::set< AstVariable* > _allVariable;
    std::set< AstVariable* > _lhsVariable;
    std::set< AstVariable* > _rhsVariable;

    size_t _indexW;
    size_t _indexH;

    bool _prevIsMatmul;

    bool isMatmul(void);
    bool isEmpty(void) const;
    bool isPragmas(void) const;
    bool isCode(void) const;
    void boundIndex(const size_t w, const size_t h);
    void boundIndex(const XStmtTogether&);
    void boundIndex(const XStmt&);
    void setIndex(const XStmtReadData&);
    void pushList(XStmt&);

public:
    XStmtTogether(void);

    void clear(void);

    const std::vector< XStmt* >& stuffInside(void) const;

    const std::set< AstVariable* >& allVariable(void) const;
    const std::set< AstVariable* >& lhsVariable(void) const;
    const std::set< AstVariable* >& rhsVariable(void) const;

    size_t indexW(void) const;
    size_t indexH(void) const;

    bool isPush(XStmt*);

    void visit(XStmtCompound&);
    void visit(XStmtCreateData&);
    void visit(XStmtGather&);
    void visit(XStmtIdSpace&);
    void visit(XStmtIndex&);
    void visit(XStmtLiteral&);
    void visit(XStmtMatmul&);
    void visit(XStmtReadData&);
    void visit(XStmtReduce&);
    void visit(XStmtRepeat&);
    void visit(XStmtRNGrand&);
    void visit(XStmtRNGseed&);
    void visit(XStmtSendData&);
    void visit(XStmtSingle&);
};

}; // namespace chai_internal

#endif
