// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_MATMUL_BASE_HPP_
#define _CHAI_X_STMT_MATMUL_BASE_HPP_

#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// base matrix multiply statement

class XStmtMatmulBase : public XStmt
{
    // C = matmul(A, B)
    enum OpType { MATMUL, MATVEC, VECMAT };
    const OpType _op;

    const AstVariable*   _lhsVariable;
    const AstMatmulBase* _matmulBase;

protected:
    XStmtMatmulBase(AstVariable*, const AstMatmulMM*);
    XStmtMatmulBase(AstVariable*, const AstMatmulMV*);
    XStmtMatmulBase(AstVariable*, const AstMatmulVM*);

public:
    virtual ~XStmtMatmulBase(void);

    bool swappable(const XStmt&) const;

    const AstVariable* astvarA(void) const;
    const AstVariable* astvarB(void) const;
    const AstVariable* astvarC(void) const;

    bool isGEMM(void) const;
    double alpha(void) const;
    double beta(void) const;

    bool isTransposeA(void) const;
    bool isTransposeB(void) const;

    bool isSameDataA(void) const;
    bool isSameDataB(void) const;

    bool isMATMUL(void) const;
    bool isMATVEC(void) const;
    bool isVECMAT(void) const;

    size_t widthA(void) const;
    size_t widthB(void) const;
    size_t widthC(void) const;

    size_t heightA(void) const;
    size_t heightB(void) const;
    size_t heightC(void) const;

    size_t precisionA(void) const;
    size_t precisionB(void) const;
    size_t precisionC(void) const;
};

}; // namespace chai_internal

#endif
