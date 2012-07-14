// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_BASE_HPP_
#define _CHAI_AST_BASE_HPP_

#include <vector>

#include "chai/RefCnt.hpp"
#include "VisitAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// base of JIT AST streams

class BaseAst : public RefObj
{
    const size_t _prec;
    const size_t _W;
    const size_t _H;
    const size_t _slots;

    // loss of referential transparency (randomness)
    const bool _randomness;

    // This may be horrible for performance.
    // Nuts own nested sub-trees of AST streams.
    // Additional ownership in the objects prevents segmentation faults.
    RefSet _refs;

    // referenced syntax tree arguments
    std::vector< BaseAst* > _argAst;

protected:
    BaseAst(const size_t PREC,
            const size_t W,
            const size_t H,
            const size_t slots,
            const bool randomness);

    void pushArg(BaseAst*);

public:
    virtual ~BaseAst(void);

    size_t W(void) const;
    size_t H(void) const;
    size_t slots(void) const;

    size_t prec(void) const;

    bool randomness(void) const;

    size_t numArg(void) const;
    BaseAst* getArg(const size_t) const;

    void replaceArg(const size_t idx, BaseAst*);

    virtual void accept(VisitAst&) = 0;
};

}; // namespace chai_internal

#endif
