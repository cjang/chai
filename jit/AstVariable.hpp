// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_VARIABLE_HPP_
#define _CHAI_AST_VARIABLE_HPP_

#include <stdint.h>

#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// represents a LHS/RHS variable

class AstVariable : public BaseAst
{
    // corresponds to trace LHS
    const bool     _isTraceVariable;
    const bool     _isLiveVariable;
    const uint32_t _variable;
    const uint32_t _version;

    // read-only and write-only variables can be image textures
    enum AccessMode
    {
        VAR_IS_READWRITE, VAR_IS_READONLY, VAR_IS_WRITEONLY
    };
    AccessMode _accessMode;

public:
    // new introduced variable not from trace
    AstVariable(BaseAst* barg);

    // variable from trace
    AstVariable(BaseAst* barg,
                const uint32_t variable,
                const uint32_t version,
                const bool isLive);

    bool isTraceVariable(void) const;
    bool isLiveVariable(void) const;
    uint32_t variable(void) const;
    uint32_t version(void) const;

    bool isReadOnly(void) const;
    bool isWriteOnly(void) const;
    void setReadOnly(void);
    void setWriteOnly(void);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
