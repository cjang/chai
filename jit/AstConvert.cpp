// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstConvert.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// convert_f32, convert_f64

// isDouble: true  false
//
// isNOP:
//   true    true  false  <----- no conversion
//   false   false true   <----- type is converted
//
AstConvert::AstConvert(BaseAst* barg,
                       const bool isDouble)
    : BaseAst(barg->W(), barg->H(), isDouble),
      _isNOP(barg->isDouble() == isDouble)
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
