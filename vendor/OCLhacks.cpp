// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <stdlib.h>

#include "Logger.hpp"
#include "OCLhacks.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// OpenCL device independent hacks

vector< string > OCLhacks::splitToken(const string& s,
                                      const char delim) const
{
    vector< string > vec;

    stringstream ss;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (delim == s[i])
        {
            if (! ss.str().empty())
                vec.push_back(ss.str());

            ss.str(string());
        }
        else
        {
            ss << s[i];
        }
    }

    if (! ss.str().empty())
        vec.push_back(ss.str());

    return vec;
}

string OCLhacks::buildVendor(void) const
{
#ifdef _VENDOR_IS_AMD_
    return _vendorAMD;
#endif

#ifdef _VENDOR_IS_INTEL_
    return _vendorINTEL;
#endif

#ifdef _VENDOR_IS_NVIDIA_
    return _vendorNVIDIA;
#endif
}

string OCLhacks::deviceName(const size_t deviceIdx)
{
    return _index2device.count(deviceIdx)
               ? _index2device[deviceIdx]
               : ""; // should not happen
}

string OCLhacks::deviceVendor(const size_t deviceIdx)
{
    stringstream ss;
    ss << deviceName(deviceIdx) << buildVendor();
    return ss.str();
}

OCLhacks::OCLhacks(void)
    : _oclInit(NULL),
      _vendorAMD("AMD"),
      _vendorINTEL("INTEL"),
      _vendorNVIDIA("NVIDIA"),
      _flagEvergreen("Evergreen"),
      _flagFP64("FP64"),
      _flagImages("Images"),
      _flagShutdownNOP("ShutdownNOP"),
      _settingParanoid("Paranoid"),
      _settingPragmaFP64("PragmaFP64"),
      _settingSearchTrials("SearchTrials"),
      _settingTimingTrials("TimingTrials"),
      _settingWatchdog("Watchdog"),
      _firstEvergreen(-1)
{ }

OCLhacks::~OCLhacks(void) { }

OCLhacks& OCLhacks::singleton(void)
{
    static OCLhacks obj;
    return obj;
}

void OCLhacks::initHacks(const OCLinit& oclInit,
                         istream& configSpec)
{
    _oclInit = &oclInit;

    // OpenCL SDKs see all compute devices but only work properly with the
    // vendor hardware. The only exception is AMD which supports both GPU and
    // CPU (made by AMD or Intel). The vendor SDK used for building the
    // application binary must match the compute device(s).
    //
    // OpenCL compute device order is undefined. It changes with driver
    // installation. Grep compute device name and vendor to find indexes.
    //
    // #device               #keywords
    // @HD5870@AMD           Cypress 5870 AMD
    // @GTX480@NVIDIA        Fermi 480 GTX
    // @Corei7920@AMD@INTEL  Core 920 Intel
    //
    // Compute devices have different capabilities.
    //
    // #device           #capability_flags
    // @HD5870           Evergreen FP64 Images
    // @GTX480           Evergreen FP64 ShutdownNOP
    // @Corei7920@AMD    Evergreen FP64
    // @Corei7920@INTEL  Evergreen FP64
    //
    // Compute devices may also have different auto-tuning settings.
    //
    // #device         #settings
    // @HD5870         Paranoid=0.1 SearchTrials=5 TimingTrials=10 Watchdog=60
    // @GTX480         Paranoid=0.1 SearchTrials=5 TimingTrials=10 Watchdog=60
    // @Corei7920      Paranoid=0.1 SearchTrials=5 TimingTrials=10 Watchdog=120
    // @Corei7920@AMD  PragmaFP64=cl_amd_fp64

    string strT, deviceKey;
    vector< string > splitT, evergreenDevices;
    bool newDevice = false;

    // read configuration
    while ( ! configSpec.eof() && (configSpec >> strT) )
    {
        // check for comment token
        if ('#' == strT[0])
        {
            continue; // skip
        }

        // check for split token key
        if ('@' == strT[0])
        {
            splitT = splitToken(strT, '@');

            deviceKey = splitT[0];

            newDevice = 0 == _deviceNames.count(deviceKey);
            _deviceNames.insert(deviceKey);

            for (size_t i = 1; i < splitT.size(); i++)
                _device2vendors[deviceKey].insert(splitT[i]);

            continue;
        }

        // is this a key/value setting?
        if (string::npos != strT.find("="))
        {
            const vector< string > settingT = splitToken(strT, '=');
            const string settingKey = settingT[0];
            const string settingValue = settingT[1];

            // is the value a number?
            stringstream ss(settingValue);
            double doubleT;
            ss >> doubleT;
            const bool isDouble = !!ss;

            if (1 == splitT.size())
            {
                // no vendors specified, applies to all
                for (set< string >::const_iterator
                     it = _device2vendors[deviceKey].begin();
                     it != _device2vendors[deviceKey].end();
                     it++)
                {
                    stringstream ss;
                    ss << deviceKey << (*it);
                    const string deviceVendor = ss.str();

                    if (isDouble)
                    {
                        _device2num[deviceVendor][settingKey] = doubleT;
                    }
                    else
                    {
                        _device2str[deviceVendor][settingKey] = settingValue;
                    }
                }
            }
            else
            {
                // vendors specified
                for (size_t i = 1; i < splitT.size(); i++)
                {
                    stringstream ss;
                    ss << deviceKey << splitT[i];
                    const string deviceVendor = ss.str();

                    if (isDouble)
                    {
                        _device2num[deviceVendor][settingKey] = doubleT;
                    }
                    else
                    {
                        _device2str[deviceVendor][settingKey] = settingValue;
                    }
                }
            }
        }
        else // device keyword or capability flag
        {
            if (newDevice)
            {
                // device keyword
                _device2keywords[deviceKey].insert(strT);
            }
            else
            {
                // capability flag
                if (1 == splitT.size())
                {
                    // no vendors specified, applies to all
                    for (set< string >::const_iterator
                         it = _device2vendors[deviceKey].begin();
                         it != _device2vendors[deviceKey].end();
                         it++)
                    {
                        stringstream ss;
                        ss << deviceKey << (*it);
                        const string deviceVendor = ss.str();

                        _device2flag[deviceVendor].insert(strT);
                    }
                }
                else
                {
                    // vendors specified
                    for (size_t i = 1; i < splitT.size(); i++)
                    {
                        stringstream ss;
                        ss << deviceKey << splitT[i];
                        const string deviceVendor = ss.str();

                        _device2flag[deviceVendor].insert(strT);
                    }
                }

                if (_flagEvergreen == strT)
                {
                    evergreenDevices.push_back(deviceKey);
                }
            }
        }
    }

    map< string, size_t > device2index;

    bool emptyDeviceDesc = false;

    // find indexes for compute devices
    for (set< string >::const_iterator
         it = _deviceNames.begin();
         it != _deviceNames.end();
         it++)
    {
        const string deviceName = *it;

        bool foundDevice = false;

        // check each OpenCL compute device
        for (size_t i = 0; i < oclInit.numberDevices(); i++)
        {
            // look for keywords in device and vendor name strings
            stringstream ss;
            ss << oclInit.devices().name(i) << " "
               << oclInit.devices().vendor(i);
            const string keywordStr = ss.str();

            if (keywordStr.empty())
                emptyDeviceDesc = true;

            // must find all keywords
            bool notFound = false;
            for (set< string >::const_iterator
                 jt = _device2keywords[deviceName].begin();
                 jt != _device2keywords[deviceName].end();
                 jt++)
            {
                if (string::npos == keywordStr.find(*jt))
                {
                    notFound = true;
                    break;
                }
            }

            if (! notFound)
            {
                // matching device
                _index2device[i] = deviceName;
                device2index[deviceName] = i;
                foundDevice = true;

                // only track compute devices compatible with the vendor SDK
                if (_device2vendors[deviceName].count(buildVendor()))
                    _deviceIndexes.insert(i);

                // take the first matching OpenCL device
                break;
            }
        }

#ifdef _LOGGING_ENABLED_
        if (! foundDevice)
        {
            stringstream ss;
            ss << "no device found for: " << deviceName;
            LOGGER(ss.str())
        }
#endif
    }

#ifdef _LOGGING_ENABLED_
    if (emptyDeviceDesc)
    {
        stringstream ss;
        ss << "found empty device description, check "
           << getenv("LD_LIBRARY_PATH");
        LOGGER(ss.str())
    }
#endif

    // first Evergreen device
    for (vector< string >::const_iterator
         it = evergreenDevices.begin();
         it != evergreenDevices.end();
         it++)
    {
        const size_t deviceIdx = device2index[*it];

        // device must be supported by vendor SDK
        if (_deviceIndexes.count(deviceIdx))
        {
            _firstEvergreen = deviceIdx;
            break;
        }
    }
}

size_t OCLhacks::firstEvergreen(void) const
{
    return _firstEvergreen;
}

bool OCLhacks::shutdownNOP(void)
{
    for (set< size_t >::const_iterator
         it = _deviceIndexes.begin();
         it != _deviceIndexes.end();
         it++)
    {
        if (shutdownNOP(*it))
            return true;
    }

    return false;
}

bool OCLhacks::shutdownNOP(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return (_device2flag.count(dv) &&
            _device2flag[dv].count(_flagShutdownNOP));
}

bool OCLhacks::supportEvergreen(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return (_device2flag.count(dv) &&
            _device2flag[dv].count(_flagEvergreen));
}

bool OCLhacks::supportFP64(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return (_device2flag.count(dv) &&
            _device2flag[dv].count(_flagFP64));
}

bool OCLhacks::supportImages(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return (_device2flag.count(dv) &&
            _device2flag[dv].count(_flagImages));
}

double OCLhacks::paranoidError(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return
        (_device2num.count(dv) && _device2num[dv].count(_settingParanoid))
            ? _device2num[dv][_settingParanoid]
            : 0; // zero round-off error, possible but unlikely
}

double OCLhacks::watchdogSecs(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return
        (_device2num.count(dv) && _device2num[dv].count(_settingWatchdog))
            ? _device2num[dv][_settingWatchdog]
            : 0; // disabled
}

size_t OCLhacks::searchTrials(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return
        (_device2num.count(dv) && _device2num[dv].count(_settingSearchTrials))
            ? _device2num[dv][_settingSearchTrials]
            : 1; // one trial only
}

size_t OCLhacks::timingTrials(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    return
        (_device2num.count(dv) && _device2num[dv].count(_settingTimingTrials))
            ? _device2num[dv][_settingTimingTrials]
            : 1; // one trial only
}

string OCLhacks::journalFile(const size_t deviceIdx)
{
    stringstream ss;

    // journal<DEVICENAME>.<VENDOR>
    ss << "journal" << deviceName(deviceIdx) << "." << buildVendor();

    return ss.str();
}

// OpenCL extension: double precision on CPUs for AMD requires cl_amd_fp64
//
// cl_khr_fp64 - GPUs with AMD and NVIDIA, CPUs with INTEL
// cl_amd_fp64 - CPUs with AMD
//
// Note cl_amd_fp64 only supports +-*/ (arithmetic) and is not full double
// precision support. However, that is still useful.
string OCLhacks::pragmaFP64(const size_t deviceIdx)
{
    const string dv = deviceVendor(deviceIdx);

    stringstream ss;
    ss << "#pragma OPENCL EXTENSION ";

    if (_device2str.count(dv) && _device2str[dv].count(_settingPragmaFP64))
    {
        ss << _device2str[dv][_settingPragmaFP64];
    }
    else
    {
        ss << "cl_khr_fp64"; // Khronos OpenCL double precision pragma
    }

    ss << " : enable";

    return ss.str();
}

const set< size_t >& OCLhacks::deviceIndexes(void) const
{
    return _deviceIndexes;
}

}; // namespace chai_internal
