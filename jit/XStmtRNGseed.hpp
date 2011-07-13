// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_RNG_SEED_HPP_
#define _CHAI_X_STMT_RNG_SEED_HPP_

#include <cstddef>

#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// initialize random number generation

class XStmtRNGseed : public XStmt
{
    const size_t _length;

public:
    XStmtRNGseed(const size_t len);

    size_t length(void) const;

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);
};

}; // namespace chai_internal

#endif
