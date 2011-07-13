// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstGather.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// gather1_floor, gather2_floor

AstGather::AstGather(BaseAst* bargData,
                     BaseAst* bargX)
    : BaseAst(bargData->W(), bargData->H(), bargData->isDouble()),
      _N(1),
      _dataW(bargData->W()),
      _dataH(bargData->H()),
      _idxW(bargX->W()),
      _idxH(bargX->H()),
      _idyW(0),
      _idyH(0)
{
    pushArg(bargData);
    pushArg(bargX);
}

AstGather::AstGather(BaseAst* bargData,
                     BaseAst* bargX,
                     BaseAst* bargY)
    : BaseAst(bargData->W(), bargData->H(), bargData->isDouble()),
      _N(2),
      _dataW(bargData->W()),
      _dataH(bargData->H()),
      _idxW(bargX->W()),
      _idxH(bargX->H()),
      _idyW(bargY->W()),
      _idyH(bargY->H())
{
    pushArg(bargData);
    pushArg(bargX);
    pushArg(bargY);
}

size_t AstGather::N(void) const
{
    return _N;
}

size_t AstGather::dataW(void) const
{
    return _dataW;
}

size_t AstGather::dataH(void) const
{
    return _dataH;
}

size_t AstGather::idxW(void) const
{
    return _idxW;
}

size_t AstGather::idxH(void) const
{
    return _idxH;
}

size_t AstGather::idyW(void) const
{
    return _idyW;
}

size_t AstGather::idyH(void) const
{
    return _idyH;
}

void AstGather::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
