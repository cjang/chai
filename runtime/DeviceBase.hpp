// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_BASE_HPP_
#define _CHAI_DEVICE_BASE_HPP_

#include <cstddef>

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
    MemManager*  _memManager;
    OCLdevice*   _oclDevice;

protected:
    DeviceBase(const size_t deviceCode);

    size_t getDeviceCode(void) const;
    MemManager& getMemManager(void) const;
    OCLdevice& getOclDevice(void) const;

public:
    ~DeviceBase(void);

    void initDevice(void);
    void initDevice(OCLinit&, const size_t deviceIndex);
    bool evaluate(VectorTrace&);

    virtual void sub_initDevice(MemManager&) = 0;
    virtual bool sub_evaluate(VectorTrace&) = 0;
};

}; // namespace chai_internal

#endif
