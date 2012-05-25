// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <limits.h>
#include <string>

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// gather1_floor, gather2_floor

void AstGather::resetCount(void)
{
    _countNode = 0;
    _countFun2Add = 0;
    _countFun2Sub = 0;
    _countIdxdataWidth = 0;
    _countIdxdataHeight = 0;
    _countScalar = 0;
    _countVariable = 0;
    _countFloorValue = INT_MAX;
}

bool AstGather::isOneVariable(void) const
{
    return
        1 == _countNode &&
        1 == _countVariable;
}

bool AstGather::isStaticSubscript(const size_t index01) const
{
    switch (_countNode)
    {
        case (1) :
            return
                (1 == _countIdxdataWidth && 0 == index01) ||
                (1 == _countIdxdataHeight && 1 == index01) ||
                1 == _countScalar;

        case (3) :
            return 
                ( (1 == _countIdxdataWidth && 0 == index01) ||
                  (1 == _countIdxdataHeight && 1 == index01) ) &&
                (1 == _countFun2Add || 1 == _countFun2Sub) &&
                1 == _countScalar;

        default:
            return false;
    }
}

void AstGather::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

AstGather::AstGather(BaseAst* bargData,
                     BaseAst* bargX)
    : BaseAst(bargData->W(), bargData->H(), bargData->precision()),
      _N(1),
      _eligible(false),
      _dataVariable(NULL),
      _xHasIndex(false),
      _yHasIndex(false),
      _xOffset(0),
      _yOffset(0)
{
    pushArg(bargData);
    pushArg(bargX);

    resetCount();
    bargData->accept(*this);
    if (isOneVariable()) // must be variable, not expression
    {
        resetCount();
        bargX->accept(*this);
        if (isStaticSubscript(0)) // if index, must be width
        {
            if (_countIdxdataWidth) _xHasIndex = true;
            if (INT_MAX != _countFloorValue)
            {
                if (_countFun2Sub)
                    _countFloorValue = -_countFloorValue;

                _xOffset = (_countFloorValue < 0)
                               ? _countFloorValue + W()
                               : _countFloorValue;
            }

            _eligible = true;
        }
    }
}

AstGather::AstGather(BaseAst* bargData,
                     BaseAst* bargX,
                     BaseAst* bargY)
    : BaseAst(bargData->W(), bargData->H(), bargData->precision()),
      _N(2),
      _eligible(false),
      _dataVariable(NULL),
      _xHasIndex(false),
      _yHasIndex(false),
      _xOffset(0),
      _yOffset(0)
{
    pushArg(bargData);
    pushArg(bargX);
    pushArg(bargY);

    resetCount();
    bargData->accept(*this);
    if (isOneVariable()) // must be variable, not expression
    {
        resetCount();
        bargX->accept(*this);
        if (isStaticSubscript(0)) // if index, must be width
        {
            if (_countIdxdataWidth) _xHasIndex = true;
            if (INT_MAX != _countFloorValue)
            {
                if (_countFun2Sub)
                    _countFloorValue = -_countFloorValue;

                _xOffset = (_countFloorValue < 0)
                               ? _countFloorValue + W()
                               : _countFloorValue;
            }

            resetCount();
            bargY->accept(*this);
            if (isStaticSubscript(1)) // if index, must be height
            {
                if (_countIdxdataHeight) _yHasIndex = true;

                if (INT_MAX != _countFloorValue)
                {
                    if (_countFun2Sub)
                        _countFloorValue = -_countFloorValue;

                    _yOffset = (_countFloorValue < 0)
                                   ? _countFloorValue + H()
                                   : _countFloorValue;
                }

                _eligible = true;
            }
        }
    }
}

size_t AstGather::N(void) const
{
    return _N;
}

bool AstGather::eligible(void) const
{
    return _eligible;
}

AstVariable* AstGather::dataVariable(void) const
{
    return _dataVariable;
}

bool AstGather::xHasIndex(void) const
{
    return _xHasIndex;
}

bool AstGather::yHasIndex(void) const
{
    return _yHasIndex;
}

size_t AstGather::xOffset(void) const
{
    return _xOffset;
}

size_t AstGather::yOffset(void) const
{
    return _yOffset;
}

////////////////////////////////////////
// VisitAst

void AstGather::visit(AstAccum& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstArrayMem&)
{
    _countNode++;
}

void AstGather::visit(AstCond& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstConvert& v)
{
    descendAst(v);
}

void AstGather::visit(AstDotprod& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstExtension& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstFun1& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstFun2& v)
{
    _countNode++;

    if (v.fun().infix())
    {
        const string opName = v.fun().str();

        if ("+" == opName)
        {
            _countFun2Add++;
        }

        if ("-" == opName)
        {
            _countFun2Sub++;
        }
    }

    descendAst(v);
}

void AstGather::visit(AstFun3& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstGather& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstIdxdata& v)
{
    _countNode++;
    if (0 == v.index()) _countIdxdataWidth++;
    if (1 == v.index()) _countIdxdataHeight++;
}

void AstGather::visit(AstLitdata& v)
{
    _countNode++;
}

void AstGather::visit(AstMakedata&)
{
    _countNode++;
}

void AstGather::visit(AstMatmulMM& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstMatmulMV& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstMatmulVM& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstMatmulVV& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstReadout&)
{
    _countNode++;
}

void AstGather::visit(AstRNGnormal&)
{
    _countNode++;
}

void AstGather::visit(AstRNGuniform&)
{
    _countNode++;
}

void AstGather::visit(AstScalar& v)
{
    _countNode++;
    _countScalar++;
    _countFloorValue = v.floorValue();
}

void AstGather::visit(AstTranspose& v)
{
    _countNode++;
    descendAst(v);
}

void AstGather::visit(AstVariable& v)
{
    if (_dataVariable)
    {
        // may really be an index
        descendAst(v);
    }
    else
    {
        _countNode++;
        _countVariable++;

        // if eligible, first time is the data variable
        _dataVariable = &v;
    }
}

void AstGather::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
