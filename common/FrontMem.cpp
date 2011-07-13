// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "FrontMem.hpp"

namespace chai_internal {

FrontMem::FrontMem(const size_t W, const size_t H, float* dataPtr)
    : RefObj(),
      _W(W),
      _H(H),
      _ptrType(FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr) { }

FrontMem::FrontMem(const size_t W, const size_t H, double* dataPtr)
    : RefObj(),
      _W(W),
      _H(H),
      _ptrType(DOUBLE),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr) { }

FrontMem::~FrontMem(void)
{
    if (_backObj)
    {
        delete _backObj->release();
    }
}

size_t FrontMem::sizeElems(void) const
{
    return _W * _H;
}

size_t FrontMem::sizeBytes(void) const
{
    switch (_ptrType)
    {
        case (FLOAT) :
            return sizeElems() * sizeof(float);

        case (DOUBLE) :
            return sizeElems() * sizeof(double);
    }
}

size_t FrontMem::W(void) const
{
    return _W;
}

size_t FrontMem::H(void) const
{
    return _H;
}

bool FrontMem::isFloat(void) const
{
    return FLOAT == _ptrType;
}

bool FrontMem::isDouble(void) const
{
    return DOUBLE == _ptrType;
}

float* FrontMem::floatPtr(void) const
{
    // swizzle data from compute device
    if (_backObj)
    {
        _backObj->swizzle();
    }

    return FLOAT == _ptrType
               ? static_cast< float* >(_ptrMem)
               : NULL;
}

double* FrontMem::doublePtr(void) const
{
    // swizzle data from compute device
    if (_backObj)
    {
        _backObj->swizzle();
    }

    return DOUBLE == _ptrType
               ? static_cast< double* >(_ptrMem)
               : NULL;
}

void* FrontMem::ptrMem(void) const
{
    return _ptrMem;
}

void* FrontMem::dataPtr(void) const
{
    return _dataPtr;
}

BackMem* FrontMem::getBackMem(void) const
{
    return _backObj;
}

void FrontMem::setBackMem(void* ptr, BackMem* obj)
{
    _ptrMem  = ptr;
    _backObj = obj;

    _backObj->checkout();
}

void FrontMem::clearBackMem(void)
{
    if (_backObj)
    {
        delete _backObj->release();

        _ptrMem  = NULL;
        _backObj = NULL;
    }
}

}; // namespace chai_internal
