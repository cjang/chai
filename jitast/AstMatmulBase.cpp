// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstMatmulBase.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// base of matmul for:
//   matrix * matrix
//   matrix * vector
//   vector * matrix

AstMatmulBase::AstMatmulBase(const size_t width,
                             const size_t height,
                             const size_t precision,
                             BaseAst* bargLeft,
                             BaseAst* bargRight)
    : BaseAst(width, height, precision),
      _isGEMM(false),
      _alpha(1),
      _beta(0),
      _transposeA(false),
      _transposeB(false),
      _sameDataA(false),
      _sameDataB(false)
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

AstMatmulBase::~AstMatmulBase(void)
{
}

size_t AstMatmulBase::leftW(void) const
{
    return getArg(0)->W();
}

size_t AstMatmulBase::leftH(void) const
{
    return getArg(0)->H();
}

size_t AstMatmulBase::leftPrecision(void) const
{
    return getArg(0)->precision();
}

size_t AstMatmulBase::rightW(void) const
{
    return getArg(1)->W();
}

size_t AstMatmulBase::rightH(void) const
{
    return getArg(1)->H();
}

size_t AstMatmulBase::rightPrecision(void) const
{
    return getArg(1)->precision();
}

void AstMatmulBase::setGEMM(AstVariable* lhsVariable,
                            AstScalar* alpha,
                            AstScalar* beta)
{
    _isGEMM = true;

    switch (alpha->precision())
    {
        case (PrecType::UInt32) : _alpha = alpha->uintValue(); break;
        case (PrecType::Int32) : _alpha = alpha->intValue(); break;
        case (PrecType::Float) : _alpha = alpha->floatValue(); break;
        case (PrecType::Double) : _alpha = alpha->doubleValue(); break;
    }

    switch (beta->precision())
    {
        case (PrecType::UInt32) : _beta = beta->uintValue(); break;
        case (PrecType::Int32) : _beta = beta->intValue(); break;
        case (PrecType::Float) : _beta = beta->floatValue(); break;
        case (PrecType::Double) : _beta = beta->doubleValue(); break;
    }

    pushArg(lhsVariable);
    pushArg(alpha);
    pushArg(beta);
}

void AstMatmulBase::setGEMM(BaseAst* lhsVariable,
                            const double alpha,
                            const double beta)
{
    _isGEMM = true;
    _alpha = alpha;
    _beta = beta;

    pushArg(lhsVariable);
    pushArg(new AstScalar(alpha));
    pushArg(new AstScalar(beta));
}

bool AstMatmulBase::isGEMM(void) const
{
    return _isGEMM;
}

double AstMatmulBase::alpha(void) const
{
    return _alpha;
}

double AstMatmulBase::beta(void) const
{
    return _beta;
}

void AstMatmulBase::setTransposeA(void)
{
    _transposeA = true;
}

void AstMatmulBase::setTransposeB(void)
{
    _transposeB = true;
}

bool AstMatmulBase::getTransposeA(void) const
{
    return _transposeA;
}

bool AstMatmulBase::getTransposeB(void) const
{
    return _transposeB;
}

void AstMatmulBase::setSameDataA(void)
{
    _sameDataA = true;
}

void AstMatmulBase::setSameDataB(void)
{
    _sameDataB = true;
}

bool AstMatmulBase::getSameDataA(void) const
{
    return _sameDataA;
}

bool AstMatmulBase::getSameDataB(void) const
{
    return _sameDataB;
}

}; // namespace chai_internal
