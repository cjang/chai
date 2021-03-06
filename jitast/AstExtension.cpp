// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstExtension.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// language extension AST object

AstExtension::AstExtension(const size_t PREC,
                           const size_t W,
                           const size_t H,
                           const size_t slots)
    : BaseAst(PREC, W, H, slots, false) { }

AstExtension::~AstExtension(void) { }

}; // namespace chai_internal
