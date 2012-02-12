// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AUTO_TUNE_MEMO_HPP_
#define _CHAI_AUTO_TUNE_MEMO_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

namespace chai_internal {

////////////////////////////////////////
// autotuning journals to a memo

class AutoTuneMemo
{
    // memo entry states for a kernel
    static const long PRINT_SOURCE    = -1;
    static const long COMPILE_KERNEL  = -2;
    static const long SET_KERNEL_ARGS = -3;
    static const long START_TIMER     = -4;
    static const long STOP_TIMER      = -5;
    static const long GOOD_OUTPUT     = -6;
    static const long BAD_OUTPUT      = -7;
    static const long BEFORE_CHECK    = -100;
    static const long AFTER_CHECK     = -101;

    // identify the device this memo is for (-1 flags ignore)
    const int _deviceNumber;

    // journal the memo to file(s)
    std::set< std::string > _journalFile;

    // entry states for all kernels
    std::map< std::string, long > _workingState;

    // known bad kernels
    std::set< std::string > _badKernels;

    // benchmark times for good kernels only
    std::map< std::string, std::vector< size_t > > _benchTime;

    void updateState(const std::string& kernelName,
                     const long stateValue);

    bool addEntry(const std::string& kernelName,
                  const long stateValue);

public:
    AutoTuneMemo(const int deviceNumber = -1);

    // journal to file(s)
    void insertJournalFile(const std::string& fileName);
    void removeJournalFile(const std::string& fileName);

    // load records from memo file(s)
    bool loadMemoFile(const std::string& fileName);
    bool loadMemoFile(const std::set< std::string >& fileName);

    // write out everything to a memo file
    bool saveMemoFile(const std::string& fileName);

    // record kernel state entries
    bool printSource(const std::string& kernelName);
    bool compileKernel(const std::string& kernelName);
    bool setKernelArgs(const std::string& kernelName);
    bool startTimer(const std::string& kernelName);
    bool stopTimer(const std::string& kernelName);
    bool beforeCheck(const std::string& kernelName);
    bool afterCheck(const std::string& kernelName);

    // record benchmark
    bool setBenchTime(const std::string& kernelName,
                      const size_t microseconds);
    bool setBenchFail(const std::string& kernelName);

    // lookup benchmark
    bool isKernelGood(const std::string& kernelName);
    bool isKernelBad(const std::string& kernelName);
    size_t getBenchTime(const std::string& kernelName,
                        const size_t trialNumber);
};

}; // namespace chai_internal

#endif
