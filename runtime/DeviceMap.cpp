// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceMap.hpp"
#include "Interpreter.hpp"
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
        const int devnum = -1;
        _allDevnums.insert( devnum );
        _deviceMap[ devnum ] = new Interpreter( devnum );
        _deviceMap[ devnum ]->initDevice();
    }

    // OpenCL compute devices
    for (set< size_t >::const_iterator
         it = OCLhacks::singleton().deviceIndexes().begin();
         it != OCLhacks::singleton().deviceIndexes().end();
         it++)
    {
        const int devnum = *it;
        _allDevnums.insert( devnum );
        _deviceMap[ devnum ] = new Translator( devnum );
        _deviceMap[ devnum ]->initDevice( oclInit );
    }
}

DeviceMap::~DeviceMap(void)
{
    for (map< int, DeviceBase* >::const_iterator
         it = _deviceMap.begin(); it != _deviceMap.end(); it++)
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
        const int devnum = -1;
        _deviceMap[ devnum ]->extendLanguage( opCode, interpHandler );
    }

    // OpenCL compute devices
    for (set< size_t >::const_iterator
         it = OCLhacks::singleton().deviceIndexes().begin();
         it != OCLhacks::singleton().deviceIndexes().end();
         it++)
    {
        const int devnum = *it;
        _deviceMap[ devnum ]->extendLanguage( opCode, jitHandler );
    }
}

const set< int >& DeviceMap::allDevnums(void) const
{
    return _allDevnums;
}

DeviceBase* DeviceMap::getDevice(const int deviceNum)
{
    return _deviceMap[ deviceNum ];
}

}; // namespace chai_internal
