// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceBase.hpp"

using namespace std;

namespace chai_internal {

int DeviceBase::getDeviceNum(void) const
{
    return _deviceNum;
}

bool DeviceBase::deviceIsCPU(void) const
{
    return _deviceIsCPU;
}

MemInterp& DeviceBase::getMemInterp(void) const
{
    return *_memMgrInterp;
}

MemTrans& DeviceBase::getMemTrans(void) const
{
    return *_memMgrTrans;
}

OCLdevice& DeviceBase::getOclDevice(void) const
{
    return *_oclDevice;
}

DeviceBase::DeviceBase(const int deviceNum)
    : _deviceNum(deviceNum),
      _deviceIsCPU(false),
      _memMgrInterp(NULL),
      _memMgrTrans(NULL),
      _oclDevice(NULL) { }

DeviceBase::~DeviceBase(void)
{
    delete _memMgrInterp;
    delete _memMgrTrans;

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
    _memMgrInterp = new MemInterp;

    // pass the memory manager to the dispatched operation objects
    sub_initDevice(*_memMgrInterp);
}

void DeviceBase::initDevice(OCLinit& oclInit)
{
    // OpenCL device object
    _oclDevice = new OCLdevice(oclInit, _deviceNum);

    // not currently used
    _deviceIsCPU = oclInit.devices().isCPU(_deviceNum);

    // note this is necessarily **after** the device object is constructed
    _memMgrTrans = new MemTrans(_deviceNum, oclInit, *_oclDevice);

    // pass the memory manager to the dispatched operation objects
    sub_initDevice(*_memMgrTrans);
}

bool DeviceBase::evaluate(VectorTrace& vt)
{
    // allocates parent memory
    const bool isOk = _memMgrTrans
                          ? _memMgrTrans->memalloc(vt) && sub_evaluate(vt)
                          : _memMgrInterp->memalloc(vt) && sub_evaluate(vt);

    // releases parent memory on failure
    if (_memMgrTrans)
        _memMgrTrans->memfree(vt, ! isOk);
    else
        _memMgrInterp->memfree(vt, ! isOk);

    return isOk;
}

void DeviceBase::sub_initDevice(MemInterp&)
{
    // do nothing, derived device will overload if it accepts this type
}

void DeviceBase::sub_initDevice(MemTrans&)
{
    // do nothing, derived device will overload if it accepts this type
}

}; // namespace chai_internal
