// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtMatmulBase.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// base matrix multiply statement

XStmtMatmulBase::XStmtMatmulBase(AstVariable* lhs,
                                 const AstMatmulMM* rhs)
    : _op(MATMUL),
      _lhsVariable(static_cast< const AstVariable* >(lhs)),
      _matmulBase(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // A on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(0)));

    // B on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(1)));

    // C on the RHS if it is there (GEMM or GEMV)
    if (rhs->isGEMM()) rhsVariable(lhs);
}

XStmtMatmulBase::XStmtMatmulBase(AstVariable* lhs,
                                 const AstMatmulMV* rhs)
    : _op(MATVEC),
      _lhsVariable(static_cast< const AstVariable* >(lhs)),
      _matmulBase(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // A on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(0)));

    // B on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(1)));

    // C on the RHS if it is there (GEMM or GEMV)
    if (rhs->isGEMM()) rhsVariable(lhs);
}

XStmtMatmulBase::XStmtMatmulBase(AstVariable* lhs,
                                 const AstMatmulVM* rhs)
    : _op(VECMAT),
      _lhsVariable(static_cast< const AstVariable* >(lhs)),
      _matmulBase(rhs)
{
    // set LHS variable
    lhsVariable(lhs);

    // A on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(0)));

    // B on the RHS
    rhsVariable(static_cast< AstVariable* >(rhs->getArg(1)));

    // C on the RHS if it is there (GEMM or GEMV)
    if (rhs->isGEMM()) rhsVariable(lhs);
}

XStmtMatmulBase::~XStmtMatmulBase(void) { }

bool XStmtMatmulBase::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

const AstVariable* XStmtMatmulBase::astvarA(void) const
{
    // operation splits in XStmtMaker ensure that all arguments are really
    // AST variable objects
    return static_cast< const AstVariable* >( _matmulBase->getArg(0) );
}

const AstVariable* XStmtMatmulBase::astvarB(void) const
{
    // operation splits in XStmtMaker ensure that all arguments are really
    // AST variable objects
    return static_cast< const AstVariable* >( _matmulBase->getArg(1) );
}

const AstVariable* XStmtMatmulBase::astvarC(void) const
{
    return _lhsVariable;
}

bool XStmtMatmulBase::isGEMM(void) const
{
    return _matmulBase->isGEMM();
}

double XStmtMatmulBase::alpha(void) const
{
    return _matmulBase->alpha();
}

double XStmtMatmulBase::beta(void) const
{
    return _matmulBase->beta();
}

bool XStmtMatmulBase::isTransposeA(void) const
{
    return _matmulBase->getTransposeA();
}

bool XStmtMatmulBase::isTransposeB(void) const
{
    return _matmulBase->getTransposeB();
}

bool XStmtMatmulBase::isSameDataA(void) const
{
    return _matmulBase->getSameDataA();
}

bool XStmtMatmulBase::isSameDataB(void) const
{
    return _matmulBase->getSameDataB();
}

bool XStmtMatmulBase::isMATMUL(void) const
{
    return MATMUL == _op;
}

bool XStmtMatmulBase::isMATVEC(void) const
{
    return MATVEC == _op;
}

bool XStmtMatmulBase::isVECMAT(void) const
{
    return VECMAT == _op;
}

size_t XStmtMatmulBase::widthA(void) const
{
    return _matmulBase->leftW();
}

size_t XStmtMatmulBase::widthB(void) const
{
    return _matmulBase->rightW();
}

size_t XStmtMatmulBase::widthC(void) const
{
    return _lhsVariable->W();
}

size_t XStmtMatmulBase::heightA(void) const
{
    return _matmulBase->leftH();
}

size_t XStmtMatmulBase::heightB(void) const
{
    return _matmulBase->rightH();
}

size_t XStmtMatmulBase::heightC(void) const
{
    return _lhsVariable->H();
}

size_t XStmtMatmulBase::precisionA(void) const
{
    return _matmulBase->leftPrecision();
}

size_t XStmtMatmulBase::precisionB(void) const
{
    return _matmulBase->rightPrecision();
}

size_t XStmtMatmulBase::precisionC(void) const
{
    return _lhsVariable->precision();
}

}; // namespace chai_internal
