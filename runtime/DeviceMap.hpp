// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_MAP_HPP_
#define _CHAI_DEVICE_MAP_HPP_

#include <cstddef>
#include <istream>
#include <map>
#include <set>
#include <stdint.h>

#include "BaseInterp.hpp"
#include "BaseTrans.hpp"
#include "DeviceBase.hpp"
#include "OCLinit.hpp"

namespace chai_internal {

////////////////////////////////////////
// all compute devices

class DeviceMap
{
    std::set< int >              _allDevnums;
    std::map< int, DeviceBase* > _deviceMap;

public:
    DeviceMap(OCLinit& oclInit, std::istream& configSpec);
    ~DeviceMap(void);

    // extend the language
    void extendLanguage(const uint32_t opCode,
                        const BaseInterp& interpHandler,
                        const BaseTrans& jitHandler);

    const std::set< int >& allDevnums(void) const;
    DeviceBase* getDevice(const int deviceNum);
};

}; // namespace chai_internal

#endif
