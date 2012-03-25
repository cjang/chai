// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_RNG_SEED_HPP_
#define _CHAI_STMT_RNG_SEED_HPP_

#include <cstddef>

#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// initialize random number generation

class StmtRNGseed : public Stmt
{
    const size_t _length;

public:
    StmtRNGseed(const size_t len);

    size_t length(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);
};

}; // namespace chai_internal

#endif
