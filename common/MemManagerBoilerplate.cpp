// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemManager.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// constructor, destructor, accessors

MemManager::MemManager(const size_t deviceCode)
    : _deviceCode(deviceCode),
      _deviceIndex(-1),
      _deviceIsCPU(true),
      _oclInit(NULL),
      _oclDevice(NULL),
      _cache(NULL)
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::MemManager(const size_t deviceCode,
                       const size_t deviceIndex,
                       OCLinit& o,
                       OCLdevice& d)
    : _deviceCode(deviceCode),
      _deviceIndex(deviceIndex),
      _deviceIsCPU(o.devices().isCPU(deviceIndex)),
      _oclInit(&o),
      _oclDevice(&d),
      _cache(new KernelCache(d))
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::~MemManager(void)
{
    pthread_mutex_destroy(&_memMutex);

    delete _cache;
}

size_t MemManager::getDeviceCode(void) const
{
    return _deviceCode;
}

size_t MemManager::getDeviceIndex(void) const
{
    return _deviceIndex;
}

OCLdevice& MemManager::getComputeDevice(void)
{
    return *_oclDevice;
}

}; // namespace chai_internal
