// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_EXTENSION_AUTO_HPP_
#define _CHAI_X_STMT_EXTENSION_AUTO_HPP_

#include "AstExtension.hpp"
#include "AstVariable.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

class XStmtExtensionAuto : public XStmt
{
    AstVariable*  _lhsVariable;
    AstExtension* _extObj;

public:
    XStmtExtensionAuto(AstVariable*, AstExtension*);

    std::string extensionName(void) const;
    bool extensionEnqueue(MemManager&, VectorTrace&);

    size_t extensionWidth(void) const;
    size_t extensionHeight(void) const;

    virtual bool swappable(const XStmt&) const;
    void accept(VisitXStmt&);
};

}; // namespace chai_internal

#endif
