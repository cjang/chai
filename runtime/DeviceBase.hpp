// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_BASE_HPP_
#define _CHAI_DEVICE_BASE_HPP_

#include <cstddef>
#include <stdint.h>

#include "BaseInterp.hpp"
#include "BaseTrans.hpp"
#include "MemInterp.hpp"
#include "MemTrans.hpp"
#include "OCLdevice.hpp"
#include "OCLinit.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// base of all compute devices

class DeviceBase
{
    const int   _deviceNum;
    bool        _deviceIsCPU;

    MemInterp*  _memMgrInterp;
    MemTrans*   _memMgrTrans;

    OCLdevice*  _oclDevice;

protected:
    int getDeviceNum(void) const;
    bool deviceIsCPU(void) const;

    MemInterp& getMemInterp(void) const;
    MemTrans& getMemTrans(void) const;

    OCLdevice& getOclDevice(void) const;

    DeviceBase(const int deviceNum);

public:
    virtual ~DeviceBase(void);

    // extend the language
    virtual bool extendLanguage(const uint32_t opCode,
                                const BaseInterp& opHandler);
    virtual bool extendLanguage(const uint32_t opCode,
                                const BaseTrans& opHandler);

    void initDevice(void);
    void initDevice(OCLinit&);

    bool evaluate(VectorTrace&);

    virtual void sub_initDevice(MemInterp&);
    virtual void sub_initDevice(MemTrans&);

    virtual bool sub_evaluate(VectorTrace&) = 0;
};

}; // namespace chai_internal

#endif
