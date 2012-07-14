// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "FrontMem.hpp"
#include "PrecType.hpp"

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
      _prec(PrecType::Float),
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
      _prec(PrecType::Double),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const size_t W,
                   const size_t H,
                   int32_t* dataPtr)
    : RefObj(),
      _variable(-1),
      _W(W),
      _H(H),
      _slots(1),
      _prec(PrecType::Int32),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const size_t W,
                   const size_t H,
                   uint32_t* dataPtr)
    : RefObj(),
      _variable(-1),
      _W(W),
      _H(H),
      _slots(1),
      _prec(PrecType::UInt32),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t PREC,
                   const size_t W,
                   const size_t H)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _prec(PREC),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t PREC,
                   const size_t W,
                   const size_t H,
                   const size_t slots)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(slots),
      _prec(PREC),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (size_t i = 0; i < slots; i++)
    {
        FrontMem* fm = new FrontMem(variable, PREC, W, H);
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
      _prec(PrecType::Float),
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
      _prec(PrecType::Double),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   int32_t* dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _prec(PrecType::Int32),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(dataPtr),
      _slotMem() { }

FrontMem::FrontMem(const uint32_t variable,
                   const size_t W,
                   const size_t H,
                   uint32_t* dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(1),
      _prec(PrecType::UInt32),
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
      _prec(PrecType::Float),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< float* >::const_iterator
         it = dataPtr.begin(); it != dataPtr.end(); it++)
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
      _prec(PrecType::Double),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< double* >::const_iterator
         it = dataPtr.begin(); it != dataPtr.end(); it++)
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
                   const vector< int32_t* >& dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(dataPtr.size()),
      _prec(PrecType::Int32),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< int32_t* >::const_iterator
         it = dataPtr.begin(); it != dataPtr.end(); it++)
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
                   const vector< uint32_t* >& dataPtr)
    : RefObj(),
      _variable(variable),
      _W(W),
      _H(H),
      _slots(dataPtr.size()),
      _prec(PrecType::UInt32),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(NULL),
      _slotMem()
{
    for (vector< uint32_t* >::const_iterator
         it = dataPtr.begin(); it != dataPtr.end(); it++)
    {
        FrontMem *fm = new FrontMem(variable, W, H, *it);
        fm->owned();
        fm->checkout(true);
        _slotMem.push_back(fm);
    }
}

FrontMem::FrontMem(const uint32_t variable,
                   void* ptr)
    : RefObj(),
      _variable(variable),
      _W(0),
      _H(0),
      _slots(0),
      _prec(-1),
      _ptrMem(NULL),
      _backObj(NULL),
      _dataPtr(ptr),
      _slotMem() { }

FrontMem::~FrontMem(void)
{
    for (vector< FrontMem* >::const_iterator
         it = _slotMem.begin(); it != _slotMem.end(); it++)
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
    return _W * _H * PrecType::sizeOf(_prec);
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

size_t FrontMem::prec(void) const
{
    return _prec;
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
    return PrecType::Float == _prec
               ? static_cast< float* >(_ptrMem)
               : NULL;
}

double* FrontMem::doublePtr(void) const
{
    return PrecType::Double == _prec
               ? static_cast< double* >(_ptrMem)
               : NULL;
}

int32_t* FrontMem::intPtr(void) const
{
    return PrecType::Int32 == _prec
               ? static_cast< int32_t* >(_ptrMem)
               : NULL;
}

uint32_t* FrontMem::uintPtr(void) const
{
    return PrecType::UInt32 == _prec
               ? static_cast< uint32_t* >(_ptrMem)
               : NULL;
}

void* FrontMem::ptrMem(void) const
{
    return _ptrMem;
}

void* FrontMem::ptrMem(const size_t PREC) const
{
    switch (PREC)
    {
        case (PrecType::UInt32) : return uintPtr();
        case (PrecType::Int32) : return intPtr();
        case (PrecType::Float) : return floatPtr();
        case (PrecType::Double) : return doublePtr();
        default: return NULL;
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
