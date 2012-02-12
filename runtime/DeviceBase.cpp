// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceBase.hpp"

using namespace std;

namespace chai_internal {

size_t DeviceBase::getDeviceCode(void) const
{
    return _deviceCode;
}

size_t DeviceBase::getDeviceIndex(void) const
{
    return _memManager->getDeviceIndex();
}

bool DeviceBase::deviceIsCPU(void) const
{
    return _deviceIsCPU;
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
      _deviceIsCPU(false),
      _memManager(NULL),
      _oclDevice(NULL) { }

DeviceBase::~DeviceBase(void)
{
    delete _memManager;
    delete _oclDevice;
}

bool DeviceBase::extendLanguage(const uint32_t opCode,
                                const BaseInterp& opHandler)
{
    // do nothing, derived device will overload if it accepts this type
    return false;
}

bool DeviceBase::extendLanguage(const uint32_t opCode,
                                const BaseTrans& opHandler)
{
    // do nothing, derived device will overload if it accepts this type
    return false;
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

    // not currently used
    _deviceIsCPU = oclInit.devices().isCPU(deviceIndex);

    // note this is necessarily **after** the device object is constructed
    _memManager = new MemManager(_deviceCode,
                                 deviceIndex,
                                 oclInit,
                                 *_oclDevice);

    // pass the memory manager to the dispatched operation objects
    sub_initDevice(*_memManager);
}

bool DeviceBase::evaluate(VectorTrace& vt)
{
    // allocates parent memory
    const bool isOk = _memManager->memalloc(vt) && sub_evaluate(vt);

    // releases parent memory on failure
    _memManager->memfree(vt, ! isOk);

    return isOk;
}

}; // namespace chai_internal
