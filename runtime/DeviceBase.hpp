// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_BASE_HPP_
#define _CHAI_DEVICE_BASE_HPP_

#include <cstddef>
#include <stdint.h>

#include "BaseInterp.hpp"
#include "BaseTrans.hpp"
#include "MemManager.hpp"
#include "OCLdevice.hpp"
#include "OCLinit.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// base of all compute devices

class DeviceBase
{
    const size_t _deviceCode;
    bool         _deviceIsCPU;
    MemManager*  _memManager;
    OCLdevice*   _oclDevice;

protected:
    DeviceBase(const size_t deviceCode);

    size_t getDeviceCode(void) const;
    size_t getDeviceIndex(void) const;
    bool deviceIsCPU(void) const;
    MemManager& getMemManager(void) const;
    OCLdevice& getOclDevice(void) const;

public:
    virtual ~DeviceBase(void);

    // extend the language
    virtual bool extendLanguage(const uint32_t opCode,
                                const BaseInterp& opHandler);
    virtual bool extendLanguage(const uint32_t opCode,
                                const BaseTrans& opHandler);

    void initDevice(void);
    void initDevice(OCLinit&, const size_t deviceIndex);
    bool evaluate(VectorTrace&);

    virtual void sub_initDevice(MemManager&) = 0;
    virtual bool sub_evaluate(VectorTrace&) = 0;
};

}; // namespace chai_internal

#endif
