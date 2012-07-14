// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_EXTENSION_HPP_
#define _CHAI_AST_EXTENSION_HPP_

#include <string>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "MemTrans.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// language extension AST object

class AstExtension : public BaseAst
{
public:
    AstExtension(const size_t PREC,
                 const size_t W,
                 const size_t H,
                 const size_t slots);

    virtual ~AstExtension(void);

    virtual void accept(VisitAst&) = 0;
    virtual std::string extensionName(void) const = 0;

    virtual bool extensionEnqueue(MemTrans&,
                                  VectorTrace&,
                                  AstVariable* lhs) = 0;

    virtual bool extensionEnqueueAuto(MemTrans&,
                                      VectorTrace&,
                                      AstVariable* lhs) = 0;
};

}; // namespace chai_internal

#endif
