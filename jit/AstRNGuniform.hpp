// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_RNG_UNIFORM_HPP_
#define _CHAI_AST_RNG_UNIFORM_HPP_

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// rng_uniform_make_f32, rng_uniform_make_f64

class AstRNGuniform : public BaseAst
{
    const unsigned int _seed;
    const size_t       _variant;
    const size_t       _step;
    const double       _minlimit;
    const double       _maxlimit;
    const bool         _isDouble;

public:
    AstRNGuniform(const unsigned int seed,
                  const size_t variant,
                  const size_t len,
                  const size_t step,
                  const double minlimit,
                  const double maxlimit,
                  const bool isDouble);

    size_t step(void) const;
    double minlimit(void) const;
    double maxlimit(void) const;

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
