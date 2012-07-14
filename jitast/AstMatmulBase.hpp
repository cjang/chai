// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_MATMUL_BASE_HPP_
#define _CHAI_AST_MATMUL_BASE_HPP_

#include "AstScalar.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// base of matmul for:
//   matrix * matrix
//   matrix * vector
//   vector * matrix
//   vector * vector

class AstMatmulBase : public BaseAst
{
    bool   _isGEMM;
    double _alpha;
    double _beta;

    bool   _transposeA;
    bool   _transposeB;

    const bool _sameDataA;
    const bool _sameDataB;

protected:
    AstMatmulBase(const size_t PREC,
                  const size_t W,
                  const size_t H,
                  const size_t slots,
                  const bool randomness,
                  BaseAst* bargLeft,
                  BaseAst* bargRight);

public:
    virtual ~AstMatmulBase(void);

    size_t leftW(void) const;
    size_t leftH(void) const;
    size_t leftSlots(void) const;
    size_t leftPrec(void) const;

    size_t rightW(void) const;
    size_t rightH(void) const;
    size_t rightSlots(void) const;
    size_t rightPrec(void) const;

    void setGEMM(AstVariable* lhsVariable,
                 AstScalar* alpha,
                 AstScalar* beta);
    void setGEMM(BaseAst* lhsVariable,
                 const double alpha,
                 const double beta);
    bool isGEMM(void) const;
    double alpha(void) const;
    double beta(void) const;

    void setTransposeA(void);
    void setTransposeB(void);
    bool getTransposeA(void) const;
    bool getTransposeB(void) const;

    bool getSameDataA(void) const;
    bool getSameDataB(void) const;
};

}; // namespace chai_internal

#endif
