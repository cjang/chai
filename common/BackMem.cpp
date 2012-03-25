// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "BackMem.hpp"
#include "Logger.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t packing,
                 const size_t frontCount,
                 float* ptr,
                 MemManager* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _packing(packing),
      _frontCount(frontCount),
      _precision(PrecType::Float),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t packing,
                 const size_t frontCount,
                 double* ptr,
                 MemManager* mm)
    : RefObj(),
      _W(W),
      _H(H),
      _packing(packing),
      _frontCount(frontCount),
      _precision(PrecType::Double),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t packing,
                 const size_t frontCount,
                 int32_t* ptr,
                 MemManager* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _packing(packing),
      _frontCount(frontCount),
      _precision(PrecType::Int32),
      _ptrMem(ptr),
      _memMgr(mm) { }

BackMem::BackMem(const size_t W,
                 const size_t H,
                 const size_t packing,
                 const size_t frontCount,
                 uint32_t* ptr,
                 MemManager* mm)
    : RefObj(false),
      _W(W),
      _H(H),
      _packing(packing),
      _frontCount(frontCount),
      _precision(PrecType::UInt32),
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
    return _W * _H * _packing;
}

size_t BackMem::W(void) const
{
    return _W;
}

size_t BackMem::H(void) const
{
    return _H;
}

size_t BackMem::packing(void) const
{
    return _packing;
}

size_t BackMem::frontCount(void) const
{
    return _frontCount;
}

size_t BackMem::precision(void) const
{
    return _precision;
}

void* BackMem::ptrMem(void) const
{
    return _ptrMem;
}

void BackMem::swizzle(void)
{
#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "this=" << this
       << " _memMgr=" << _memMgr
       << " W=" << _W
       << " H=" << _H;
    LOGGER(ss.str())
#endif

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
