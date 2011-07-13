// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstRNGuniform.hpp"

namespace chai_internal {

////////////////////////////////////////////////
// rng_uniform_make_f32, rng_uniform_make_f64

AstRNGuniform::AstRNGuniform(const unsigned int seed,
                             const size_t variant,
                             const size_t len,
                             const size_t step,
                             const double minlimit,
                             const double maxlimit,
                             const bool isDouble)
    : BaseAst(len, 1, isDouble),
      _seed(seed),
      _variant(variant),
      _step(step),
      _minlimit(minlimit),
      _maxlimit(maxlimit),
      _isDouble(isDouble) { }

size_t AstRNGuniform::step(void) const
{
    return _step;
}

double AstRNGuniform::minlimit(void) const
{
    return _minlimit;
}

double AstRNGuniform::maxlimit(void) const
{
    return _maxlimit;
}

void AstRNGuniform::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
