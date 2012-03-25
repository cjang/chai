// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstConvert.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// convert_f32, convert_f64

AstConvert::AstConvert(BaseAst* barg,
                       const size_t precision)
    : BaseAst(barg->W(), barg->H(), precision),
      _isNOP(barg->precision() == precision)
{
    pushArg(barg);
}

bool AstConvert::isNOP(void) const
{
    return _isNOP;
}

void AstConvert::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
