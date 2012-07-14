// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_EXTENSION_AUTO_HPP_
#define _CHAI_STMT_EXTENSION_AUTO_HPP_

#include "AstExtension.hpp"
#include "AstVariable.hpp"
#include "MemTrans.hpp"
#include "VectorTrace.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

class StmtExtensionAuto : public Stmt
{
    AstVariable*  _lhsVariable;
    AstExtension* _extObj;

public:
    StmtExtensionAuto(AstVariable*, AstExtension*);

    std::string extensionName(void) const;
    bool extensionEnqueueAuto(MemTrans&, VectorTrace&);

    size_t extensionWidth(void) const;
    size_t extensionHeight(void) const;

    bool randomness(void) const;

    virtual bool swappable(const Stmt&) const;
    void accept(VisitStmt&);
};

}; // namespace chai_internal

#endif
