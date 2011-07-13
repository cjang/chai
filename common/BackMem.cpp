// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BackMem.hpp"
#include "MemManager.hpp"

namespace chai_internal {

BackMem::BackMem(const size_t W, const size_t H, float* ptr, MemManager* mm)
    : RefObj(),
      _W(W),
      _H(H),
      _ptrType(FLOAT),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W, const size_t H, double* ptr, MemManager* mm)
    : RefObj(),
      _W(W),
      _H(H),
      _ptrType(DOUBLE),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::~BackMem(void)
{
    switch (_ptrType)
    {
        case (FLOAT) :
            free(static_cast< float* >(_ptrMem));
            break;

        case (DOUBLE) :
            free(static_cast< double* >(_ptrMem));
            break;
    }

    // inform memory manager of object's destruction
    if (_memMgr)
    {
        _memMgr->untrackMem(this);
    }
}

size_t BackMem::sizeElems(void) const
{
    return _W * _H;
}

size_t BackMem::sizeBytes(void) const
{
    switch (_ptrType)
    {
        case (FLOAT) :
            return sizeElems() * sizeof(float);

        case (DOUBLE) :
            return sizeElems() * sizeof(double);
    }
}

size_t BackMem::W(void) const
{
    return _W;
}

size_t BackMem::H(void) const
{
    return _H;
}

bool BackMem::isFloat(void) const
{
    return FLOAT == _ptrType;
}

bool BackMem::isDouble(void) const
{
    return DOUBLE == _ptrType;
}

void* BackMem::ptrMem(void) const
{
    return _ptrMem;
}

void BackMem::swizzle(void)
{
    // memory manager knows to only move data once
    if (_memMgr)
    {
        _memMgr->swizzle(this);
    }
}

}; // namespace chai_internal
