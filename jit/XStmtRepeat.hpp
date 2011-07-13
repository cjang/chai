// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_REPEAT_HPP_
#define _CHAI_X_STMT_REPEAT_HPP_

#include <cstddef>

#include "XStmt.hpp"
#include "XStmtCompound.hpp"

namespace chai_internal {

////////////////////////////////////////
// repeat AST statement

class XStmtRepeat : public XStmt
{
    XStmtCompound* _comStmt;

    const size_t   _reps;

    bool           _loopRoll;

public:
    XStmtRepeat(const size_t reps);
    ~XStmtRepeat(void);

    void setCompound(XStmtCompound*);

    bool getLoopRoll(void) const;
    void setLoopRoll(const bool);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    size_t numReps(void) const;

    XStmtCompound* stuffInside(void) const;
};

}; // namespace chai_internal

#endif
