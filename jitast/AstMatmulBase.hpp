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

class AstMatmulBase : public BaseAst
{
    bool   _isGEMM;
    double _alpha;
    double _beta;
    bool   _transposeA;
    bool   _transposeB;
    bool   _sameDataA;
    bool   _sameDataB;

protected:
    AstMatmulBase(const size_t width,
                  const size_t height,
                  const size_t precision,
                  BaseAst* bargLeft,
                  BaseAst* bargRight);

public:
    virtual ~AstMatmulBase(void);

    size_t leftW(void) const;
    size_t leftH(void) const;
    size_t leftPrecision(void) const;

    size_t rightW(void) const;
    size_t rightH(void) const;
    size_t rightPrecision(void) const;

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

    void setSameDataA(void);
    void setSameDataB(void);
    bool getSameDataA(void) const;
    bool getSameDataB(void) const;
};

}; // namespace chai_internal

#endif
