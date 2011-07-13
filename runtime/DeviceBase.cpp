// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceBase.hpp"

using namespace std;

namespace chai_internal {

size_t DeviceBase::getDeviceCode(void) const
{
    return _deviceCode;
}

MemManager& DeviceBase::getMemManager(void) const
{
    return *_memManager;
}

OCLdevice& DeviceBase::getOclDevice(void) const
{
    return *_oclDevice;
}

DeviceBase::DeviceBase(const size_t deviceCode)
    : _deviceCode(deviceCode),
      _memManager(NULL),
      _oclDevice(NULL) { }

DeviceBase::~DeviceBase(void)
{
    delete _memManager;
    delete _oclDevice;
}

void DeviceBase::initDevice(void)
{
    // note this is necessarily **after** the device object is constructed
    _memManager = new MemManager(_deviceCode);

    // pass the memory manager to the dispatched operation objects
    sub_initDevice(*_memManager);
}

void DeviceBase::initDevice(OCLinit& oclInit,
                            const size_t deviceIndex)
{
    // OpenCL device object
    _oclDevice = new OCLdevice(oclInit, deviceIndex);

    // note this is necessarily **after** the device object is constructed
    _memManager = new MemManager(_deviceCode, oclInit, *_oclDevice);

    // pass the memory manager to the dispatched operation objects
    sub_initDevice(*_memManager);
}

bool DeviceBase::evaluate(VectorTrace& vt)
{
    // allocates parent memory
    _memManager->memalloc(vt);

    const bool isOk = sub_evaluate(vt);

    // releases parent memory on failure
    _memManager->memfree(vt, ! isOk);

    return isOk;
}

}; // namespace chai_internal
