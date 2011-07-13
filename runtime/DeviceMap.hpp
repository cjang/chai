// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_MAP_HPP_
#define _CHAI_DEVICE_MAP_HPP_

#include <cstddef>
#include <map>
#include <set>

#include "DeviceBase.hpp"
#include "OCLinit.hpp"

namespace chai_internal {

////////////////////////////////////////
// number codes for all compute devices

class DeviceMap
{
    std::set< size_t >              _allCodes;
    std::map< size_t, DeviceBase* > _codeDevice;

public:
    DeviceMap(OCLinit&);

    const std::set< size_t >& allCodes(void) const;
    DeviceBase* getDevice(const size_t deviceCode);
};

}; // namespace chai_internal

#endif
