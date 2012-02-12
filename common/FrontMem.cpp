// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "FrontMem.hpp"

using namespace std;

namespace chai_internal {

FrontMem::FrontMem(const size_t W,
                   const size_t H,
                   float* dataPtr)
    : RefObj(),
      _variable(-1),
      _W(W),
      _H(H),
      _slots(1),
      _ptrType(FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const size_t W,
                   const size_t H,
                   double* dataPtr)
    : RefObj(),
      _variable(-1),
      _W(W),
      _H(H),
      _slots(1),
      _ptrType(DOUBLE),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   const size_t precision)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _ptrType(sizeof(double) == precision ? DOUBLE : FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   const size_t precision,
                   const size_t slots)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(slots),
      _ptrType(sizeof(double) == precision ? DOUBLE : FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (size_t i = 0; i < slots; i++)
    {
        FrontMem* fm = new FrontMem(variable, W, H, precision);
        fm->owned();
        fm->checkout(true);
        _slotMem.push_back(fm);
    }
}

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   float* dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _ptrType(FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   double* dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _ptrType(DOUBLE),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   const vector< float* >& dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(dataPtr.size()),
      _ptrType(FLOAT),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< float* >::const_iterator
         it = dataPtr.begin();
         it != dataPtr.end();
         it++)
    {
        FrontMem *fm = new FrontMem(variable, W, H, *it);
        fm->owned();
        fm->checkout(true);
        _slotMem.push_back(fm);
    }
}

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   const vector< double* >& dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(dataPtr.size()),
      _ptrType(DOUBLE),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< double* >::const_iterator
         it = dataPtr.begin();
         it != dataPtr.end();
         it++)
    {
        FrontMem *fm = new FrontMem(variable, W, H, *it);
        fm->owned();
        fm->checkout(true);
        _slotMem.push_back(fm);
    }
}

FrontMem::~FrontMem(void)
{
    for (vector< FrontMem* >::const_iterator
         it = _slotMem.begin();
         it != _slotMem.end();
         it++)
    {
        delete *it;
    }

    if (_backObj)
    {
        delete _backObj->release();
    }
}

size_t FrontMem::sizeBytes(void) const
{
    switch (_ptrType)
    {
        case (FLOAT) :
            return _W * _H * sizeof(float);

        case (DOUBLE) :
            return _W * _H * sizeof(double);
    }
}

uint32_t FrontMem::variable(void) const
{
    return _variable;
}

size_t FrontMem::W(void) const
{
    return _W;
}

size_t FrontMem::H(void) const
{
    return _H;
}

size_t FrontMem::slots(void) const
{
    return _slots;
}

bool FrontMem::isFloat(void) const
{
    return FLOAT == _ptrType;
}

bool FrontMem::isDouble(void) const
{
    return DOUBLE == _ptrType;
}

void FrontMem::swizzle(const size_t uniqueKey)
{
    // swizzle data from compute device
    if (_backObj)
    {
        _backObj->swizzle(uniqueKey);
    }
}

float* FrontMem::floatPtr(void) const
{
    return FLOAT == _ptrType
               ? static_cast< float* >(_ptrMem)
               : NULL;
}

double* FrontMem::doublePtr(void) const
{
    return DOUBLE == _ptrType
               ? static_cast< double* >(_ptrMem)
               : NULL;
}

void* FrontMem::ptrMem(void) const
{
    return _ptrMem;
}

void* FrontMem::ptrMem(const size_t precision) const
{
    if (precision == sizeof(double))
    {
        return doublePtr();
    }
    else
    {
        return floatPtr();
    }
}

void* FrontMem::dataPtr(void) const
{
    return _dataPtr;
}

const vector< FrontMem* >& FrontMem::slotMem(void) const
{
    return _slotMem;
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
