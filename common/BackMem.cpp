// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdlib.h>

#include "BackMem.hpp"
#include "MemCallback.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t slots,
                 const size_t frontCount,
                 float* ptr,
                 MemCallback* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _slots(slots),
      _frontCount(frontCount),
      _prec(PrecType::Float),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t slots,
                 const size_t frontCount,
                 double* ptr,
                 MemCallback* mm)
    : RefObj(),
      _W(W),
      _H(H),
      _slots(slots),
      _frontCount(frontCount),
      _prec(PrecType::Double),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t slots,
                 const size_t frontCount,
                 int32_t* ptr,
                 MemCallback* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _slots(slots),
      _frontCount(frontCount),
      _prec(PrecType::Int32),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t slots,
                 const size_t frontCount,
                 uint32_t* ptr,
                 MemCallback* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _slots(slots),
      _frontCount(frontCount),
      _prec(PrecType::UInt32),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::~BackMem(void)
{
    free(_ptrMem);

    // inform memory manager of object's destruction
    if (_memMgr)
    {
        _memMgr->untrackMem(this);
    }
}

size_t BackMem::sizeElems(void) const
{
    return _W * _H * _slots;
}

size_t BackMem::W(void) const
{
    return _W;
}

size_t BackMem::H(void) const
{
    return _H;
}

size_t BackMem::slots(void) const
{
    return _slots;
}

size_t BackMem::frontCount(void) const
{
    return _frontCount;
}

size_t BackMem::prec(void) const
{
    return _prec;
}

void* BackMem::ptrMem(void) const
{
    return _ptrMem;
}

void BackMem::swizzle(void)
{
    // Translator scheduling explicitly transfers memory to and from compute
    // devices with read_scalar(), read1() and read2() calls in the execution
    // trace. There is only one agent that swizzles, the translator itself.
    if (_memMgr)
    {
        _memMgr->swizzle(this);
    }
}

void BackMem::swizzle(const size_t uniqueKey)
{
    // Interpreter scheduling following the translator may require implicit
    // data transfers from the compute device back to the CPU host. Swizzle
    // calls are through front memory. A unique key for every interpreter
    // initiated swizzle across a vector of front memory allows the backing
    // memory objects (of which there may be multiple) to elide redundant
    // swizzle calls to the memory managers.
    //
    // Regardless, swizzling is always the movement of data from an OpenCL
    // compute device back to the CPU host.
    if (_memMgr)
    {
        if (0 == _swizzleSet.count(uniqueKey))
        {
            _swizzleSet.insert(uniqueKey);

            _memMgr->swizzle(this);
        }
    }
}

}; // namespace chai_internal
