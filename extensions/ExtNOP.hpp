// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EXT_NOP_HPP_
#define _CHAI_EXT_NOP_HPP_

#include "AstExtension.hpp"

namespace chai_internal {

////////////////////////////////////////
// language extension object

class ExtNOP : public AstExtension
{
public:
    ExtNOP(BaseAst* barg);

    void accept(VisitAst&);

    std::string extensionName(void) const;
    bool extensionEnqueue(MemManager&, VectorTrace&, AstVariable*);
    bool extensionEnqueueAuto(MemManager&, VectorTrace&, AstVariable*);
};

}; // namespace chai_internal

#endif
