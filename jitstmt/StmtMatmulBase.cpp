// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtMatmulBase.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// base matrix multiply statement

StmtMatmulBase::StmtMatmulBase(AstVariable* lhs,
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

StmtMatmulBase::StmtMatmulBase(AstVariable* lhs,
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

StmtMatmulBase::StmtMatmulBase(AstVariable* lhs,
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

StmtMatmulBase::~StmtMatmulBase(void) { }

bool StmtMatmulBase::randomness(void) const
{
    return false; // matmul() statements always separate kernels
}

bool StmtMatmulBase::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

const AstVariable* StmtMatmulBase::astvarA(void) const
{
    // operation splits in StmtMaker ensure that all arguments are really
    // AST variable objects
    return static_cast< const AstVariable* >( _matmulBase->getArg(0) );
}

const AstVariable* StmtMatmulBase::astvarB(void) const
{
    // operation splits in StmtMaker ensure that all arguments are really
    // AST variable objects
    return static_cast< const AstVariable* >( _matmulBase->getArg(1) );
}

const AstVariable* StmtMatmulBase::astvarC(void) const
{
    return _lhsVariable;
}

bool StmtMatmulBase::isGEMM(void) const
{
    return _matmulBase->isGEMM();
}

double StmtMatmulBase::alpha(void) const
{
    return _matmulBase->alpha();
}

double StmtMatmulBase::beta(void) const
{
    return _matmulBase->beta();
}

bool StmtMatmulBase::isTransposeA(void) const
{
    return _matmulBase->getTransposeA();
}

bool StmtMatmulBase::isTransposeB(void) const
{
    return _matmulBase->getTransposeB();
}

bool StmtMatmulBase::isSameDataA(void) const
{
    return _matmulBase->getSameDataA();
}

bool StmtMatmulBase::isSameDataB(void) const
{
    return _matmulBase->getSameDataB();
}

bool StmtMatmulBase::isMATMUL(void) const
{
    return MATMUL == _op;
}

bool StmtMatmulBase::isMATVEC(void) const
{
    return MATVEC == _op;
}

bool StmtMatmulBase::isVECMAT(void) const
{
    return VECMAT == _op;
}

size_t StmtMatmulBase::widthA(void) const
{
    return _matmulBase->leftW();
}

size_t StmtMatmulBase::widthB(void) const
{
    return _matmulBase->rightW();
}

size_t StmtMatmulBase::widthC(void) const
{
    return _lhsVariable->W();
}

size_t StmtMatmulBase::heightA(void) const
{
    return _matmulBase->leftH();
}

size_t StmtMatmulBase::heightB(void) const
{
    return _matmulBase->rightH();
}

size_t StmtMatmulBase::heightC(void) const
{
    return _lhsVariable->H();
}

size_t StmtMatmulBase::precA(void) const
{
    return _matmulBase->leftPrec();
}

size_t StmtMatmulBase::precB(void) const
{
    return _matmulBase->rightPrec();
}

size_t StmtMatmulBase::precC(void) const
{
    return _lhsVariable->prec();
}

}; // namespace chai_internal
