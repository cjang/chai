// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <omp.h>

#include "DeviceMap.hpp"
#include "Interpreter.hpp"
#include "MemManager.hpp"
#include "Translator.hpp"

using namespace std;

namespace chai_internal {

DeviceMap::DeviceMap(OCLinit& oclInit)
{
    // interpreter on one CPU core
    for (size_t coreIndex = 0;
         coreIndex < 1;
         coreIndex++)
    {
        const size_t deviceCode = MemManager::CPU_INTERP + coreIndex;
        _allCodes.insert( deviceCode );
        _codeDevice[ deviceCode ] = new Interpreter(deviceCode);
        _codeDevice[ deviceCode ]->initDevice();
    }

    // OpenCL compute device
    for (size_t deviceIndex = 0;
         deviceIndex < oclInit.numberDevices();
         deviceIndex++)
    {
        const size_t deviceCode = MemManager::GPU_OPENCL + deviceIndex;
        _allCodes.insert( deviceCode );
        _codeDevice[ deviceCode ] = new Translator(deviceCode);
        _codeDevice[ deviceCode ]->initDevice(oclInit, deviceIndex);
    }
}

const set< size_t >& DeviceMap::allCodes(void) const
{
    return _allCodes;
}

DeviceBase* DeviceMap::getDevice(const size_t deviceCode)
{
    return _codeDevice[deviceCode];
}

}; // namespace chai_internal
