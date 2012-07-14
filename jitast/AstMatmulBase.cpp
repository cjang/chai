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

AstMatmulBase::AstMatmulBase(const size_t PREC,
                             const size_t W,
                             const size_t H,
                             const size_t slots,
                             const bool randomness,
                             BaseAst* bargLeft,
                             BaseAst* bargRight)
    : BaseAst(PREC, W, H, slots, randomness),
      _isGEMM(false),
      _alpha(1),
      _beta(0),
      _transposeA(false),
      _transposeB(false),
      _sameDataA(1 == bargLeft->slots()),
      _sameDataB(1 == bargRight->slots())
{
    pushArg(bargLeft);
    pushArg(bargRight);
}

AstMatmulBase::~AstMatmulBase(void) { }

size_t AstMatmulBase::leftW(void) const
{
    return getArg(0)->W();
}

size_t AstMatmulBase::leftH(void) const
{
    return getArg(0)->H();
}

size_t AstMatmulBase::leftSlots(void) const
{
    return getArg(0)->slots();
}

size_t AstMatmulBase::leftPrec(void) const
{
    return getArg(0)->prec();
}

size_t AstMatmulBase::rightW(void) const
{
    return getArg(1)->W();
}

size_t AstMatmulBase::rightH(void) const
{
    return getArg(1)->H();
}

size_t AstMatmulBase::rightSlots(void) const
{
    return getArg(1)->slots();
}

size_t AstMatmulBase::rightPrec(void) const
{
    return getArg(1)->prec();
}

void AstMatmulBase::setGEMM(AstVariable* lhsVariable,
                            AstScalar* alpha,
                            AstScalar* beta)
{
    _isGEMM = true;

    switch (alpha->prec())
    {
        case (PrecType::UInt32) : _alpha = alpha->uintValue(); break;
        case (PrecType::Int32) : _alpha = alpha->intValue(); break;
        case (PrecType::Float) : _alpha = alpha->floatValue(); break;
        case (PrecType::Double) : _alpha = alpha->doubleValue(); break;
    }

    switch (beta->prec())
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

bool AstMatmulBase::getSameDataA(void) const
{
    return _sameDataA;
}

bool AstMatmulBase::getSameDataB(void) const
{
    return _sameDataB;
}

}; // namespace chai_internal
