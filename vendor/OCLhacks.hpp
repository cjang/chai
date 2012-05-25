// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_OCL_HACKS_HPP_
#define _CHAI_OCL_HACKS_HPP_

#include <istream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "OCLinit.hpp"

namespace chai_internal {

////////////////////////////////////////
// OpenCL device dependent hacks

class OCLhacks
{
    const std::string _vendorAMD;
    const std::string _vendorINTEL;
    const std::string _vendorNVIDIA;

    const std::string _flagEvergreen;
    const std::string _flagFP64;
    const std::string _flagImages;
    const std::string _flagShutdownNOP;

    const std::string _settingParanoid;
    const std::string _settingPragmaFP64;
    const std::string _settingSearchTrials;
    const std::string _settingTimingTrials;
    const std::string _settingWatchdog;
    const std::string _settingWorkGroupSize;

    const OCLinit* _oclInit;

    std::set< std::string > _deviceNames; // device name not including vendor
    std::map< std::string, std::set< std::string > > _device2keywords;
    std::map< std::string, std::set< std::string > > _device2vendors;

    std::set< std::string > _flagNames;
    std::map< std::string, std::set< std::string > >              _device2flag;
    std::map< std::string, std::map< std::string, double > >      _device2num;
    std::map< std::string, std::map< std::string, std::string > > _device2str;

    std::set< size_t > _deviceIndexes;
    std::map< size_t, std::string > _index2device;

    size_t _firstEvergreen;

    std::vector< std::string > splitToken(const std::string& s,
                                          const char delim) const;

    std::string buildVendor(void) const;
    std::string deviceName(const size_t deviceIdx);
    std::string deviceVendor(const size_t deviceIndex);

    OCLhacks(void);

public:
    ~OCLhacks(void);

    static OCLhacks& singleton(void);

    void initHacks(const OCLinit& oclInit,
                   std::istream& configSpec);

    size_t firstEvergreen(void) const; // first Evergreen compute device

    bool shutdownNOP(void);
    bool shutdownNOP(const size_t deviceIdx);
    bool supportEvergreen(const size_t deviceIdx);
    bool supportFP64(const size_t deviceIdx);
    bool supportImages(const size_t deviceIdx);
    double paranoidError(const size_t deviceIdx);
    double watchdogSecs(const size_t deviceIdx);
    size_t searchTrials(const size_t deviceIdx);
    size_t timingTrials(const size_t deviceIdx);
    size_t workGroupSize(const size_t deviceIdx);
    std::string journalFile(const size_t deviceIdx);
    std::string pragmaFP64(const size_t deviceIdx);

    const std::set< size_t >& deviceIndexes(void) const;
};

}; // namespace chai_internal

#endif
