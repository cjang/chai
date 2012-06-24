// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceMap.hpp"
#include "Interpreter.hpp"
#include "MemManager.hpp"
#include "OCLhacks.hpp"
#include "Translator.hpp"

using namespace std;

namespace chai_internal {

DeviceMap::DeviceMap(OCLinit& oclInit, istream& configSpec)
{
    // this must occur before creating the device handling objects below
    OCLhacks::singleton().initHacks(oclInit, configSpec);

    if (OCLhacks::singleton().enableInterpreter())
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
    }

    // OpenCL compute device
    for (set< size_t >::const_iterator
         it = OCLhacks::singleton().deviceIndexes().begin();
         it != OCLhacks::singleton().deviceIndexes().end();
         it++)
    {
        const size_t deviceIndex = *it;
        const size_t deviceCode = MemManager::GPU_OPENCL + deviceIndex;
        _allCodes.insert( deviceCode );
        _codeDevice[ deviceCode ] = new Translator(deviceCode);
        _codeDevice[ deviceCode ]->initDevice(oclInit, deviceIndex);
    }
}

DeviceMap::~DeviceMap(void)
{
    for (map<size_t, DeviceBase* >::const_iterator
         it = _codeDevice.begin();
         it != _codeDevice.end();
         it++)
    {
        delete (*it).second;
    }
}

void DeviceMap::extendLanguage(const uint32_t opCode,
                               const BaseInterp& interpHandler,
                               const BaseTrans& jitHandler)
{
    if (OCLhacks::singleton().enableInterpreter())
    {
        // interpreter on one CPU core
        for (size_t coreIndex = 0;
             coreIndex < 1;
             coreIndex++)
        {
            const size_t deviceCode = MemManager::CPU_INTERP + coreIndex;

            _codeDevice[ deviceCode ]->extendLanguage(opCode,
                                                      interpHandler);
        }
    }

    // OpenCL compute device
    for (set< size_t >::const_iterator
         it = OCLhacks::singleton().deviceIndexes().begin();
         it != OCLhacks::singleton().deviceIndexes().end();
         it++)
    {
        const size_t deviceIndex = *it;
        const size_t deviceCode = MemManager::GPU_OPENCL + deviceIndex;

        _codeDevice[ deviceCode ]->extendLanguage(opCode,
                                                  jitHandler);
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
