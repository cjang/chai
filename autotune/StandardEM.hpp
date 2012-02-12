// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STANDARD_EM_HPP_
#define _CHAI_STANDARD_EM_HPP_

#include <ostream>
#include <set>
#include <string>

#include "AutoTuneBench.hpp"
#include "AutoTuneMemo.hpp"
#include "KernelBase.hpp"
#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// standard application use of EM

class StandardEM
{
    OCLdevice& _cdev;

    AutoTuneMemo  _searchJournal;
    AutoTuneBench _searchBench;

    size_t _numberSearchTrials;
    size_t _numberTimingTrials;

    bool _includeSendDataToDevice;
    bool _includeReadDataFromDevice;

    std::ostream* _os;

public:
    StandardEM(OCLdevice& cdev);

    void setJournalFile(const std::string& journalFile);
    void setJournalFile(const std::set< std::string >& journalFileSet);

    // print status messages
    void setDebug(std::ostream& os);
    void setDebug(void);

    // search - number of trials per parameter combination during EM
    // final - number of trials for paranoid check and final benchmark
    void setNumberTrials(const size_t searchTrials,
                         const size_t timingTrials);

    void setIncludeSendDataToDevice(const bool setting = true);
    void setIncludeReadDataFromDevice(const bool setting = true);

    // expectation-maximization search
    bool searchLoop(KernelBase& kernelGen);

    // returns GFLOPS
    double timingLoop(KernelBase& kernelGen);

    // catch any hangs or crashes during kernel checking
    void beforeCheck(const KernelBase& kernelGen);
    void afterCheck(const KernelBase& kernelGen);
};

}; // namespace chai_internal

#endif
