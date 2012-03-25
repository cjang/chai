// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_REPEAT_HPP_
#define _CHAI_STMT_REPEAT_HPP_

#include <cstddef>

#include "Stmt.hpp"
#include "StmtCompound.hpp"

namespace chai_internal {

////////////////////////////////////////
// repeat AST statement

class StmtRepeat : public Stmt
{
    StmtCompound* _comStmt;

    const size_t  _reps;

    bool          _loopRoll;

public:
    StmtRepeat(const size_t reps);
    ~StmtRepeat(void);

    void setCompound(StmtCompound*);

    bool getLoopRoll(void) const;
    void setLoopRoll(const bool);

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    size_t numReps(void) const;

    StmtCompound* stuffInside(void) const;
};

}; // namespace chai_internal

#endif
