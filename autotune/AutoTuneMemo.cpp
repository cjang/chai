// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>
#include <sstream>
#include <sys/time.h>

#include "AutoTuneMemo.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// AutoTuneMemo

void AutoTuneMemo::updateState(const string& kernelName,
                               const long stateValue)
{
    // negative values are states
    if (stateValue < 0)
    {
        bool badKernelDetected = BAD_OUTPUT == stateValue;

        // check for failed compiles, hung kernels and bad output

        if (0 == _workingState.count(kernelName))
        {
            // first memo state for a kernel should be printing source
            if (PRINT_SOURCE != stateValue)
                badKernelDetected = true;
        }
        else
        {
            // check for valid state transitions
            switch (_workingState[kernelName])
            {
                case (PRINT_SOURCE) :
                    if (COMPILE_KERNEL != stateValue)
                        badKernelDetected = true;
                    break;

                case (COMPILE_KERNEL) :
                    if (SET_KERNEL_ARGS != stateValue)
                        badKernelDetected = true;
                    break;

                case (SET_KERNEL_ARGS) :
                    if (START_TIMER != stateValue)
                        badKernelDetected = true;
                    break;

                case (START_TIMER) :
                    if (STOP_TIMER != stateValue)
                        badKernelDetected = true;
                    break;

                case (STOP_TIMER) :
                    if (GOOD_OUTPUT != stateValue)
                        badKernelDetected = true;
                    break;

                case (GOOD_OUTPUT) :
                    if (PRINT_SOURCE != stateValue &&
                        BEFORE_CHECK != stateValue)
                        badKernelDetected = true;
                    break;

                case (BAD_OUTPUT) :
                    // kernel is already marked as bad
                    break;

                case (BEFORE_CHECK) :
                    if (AFTER_CHECK != stateValue)
                        badKernelDetected = true;
                    break;

                case (AFTER_CHECK) :
                    if (PRINT_SOURCE != stateValue &&
                        BEFORE_CHECK != stateValue)
                        badKernelDetected = true;
                    break;
            }
        }

        if (badKernelDetected)
            _badKernels.insert(kernelName);
    }

    if (stateValue < 0)
    {
        // negative values are states
        _workingState[kernelName] = stateValue;
    }
    else
    {
        // non-negative values are benchmark times
        _benchTime[kernelName].push_back(stateValue);
    }
}

bool AutoTuneMemo::addEntry(const string& kernelName,
                            const long stateValue)
{
    // update memo
    updateState(kernelName, stateValue);

    // journal to files
    bool rc = true;

    for (set< string >::const_iterator
         it = _journalFile.begin(); it != _journalFile.end(); it++)
    {
        ofstream ofs((*it).c_str(), ios::app);

        if (ofs.is_open())
        {
            ofs << _deviceNumber << " "
                << kernelName << " "
                << stateValue
                << endl;
        }
        else
        {
            // failed to open a journal file
            rc = false;
        }
    }

    return rc;
}

AutoTuneMemo::AutoTuneMemo(const int deviceNumber)
    : _deviceNumber(deviceNumber),
      _journalFile(),
      _workingState(),
      _badKernels(),
      _benchTime() { }

void AutoTuneMemo::insertJournalFile(const string& fileName)
{
    _journalFile.insert(fileName);
}

void AutoTuneMemo::removeJournalFile(const string& fileName)
{
    _journalFile.erase(fileName);
}

bool AutoTuneMemo::loadMemoFile(const string& fileName)
{
    ifstream ifs(fileName.c_str());

    if (ifs.is_open())
    {
        size_t deviceNumber;
        string kernelName;
        long stateValue;

        while ( ! ifs.eof() &&
                (ifs >> deviceNumber >> kernelName >> stateValue) )
        {
            if (_deviceNumber == deviceNumber)
                updateState(kernelName, stateValue);
        }

        // after loading, any kernels in states:
        //   COMPILE_KERNEL
        //   START_TIMER
        //   BEFORE_CHECK
        // must have hung or crashed
        for (map< string, long >::const_iterator
             it = _workingState.begin(); it != _workingState.end(); it++)
        {
            if (COMPILE_KERNEL == (*it).second ||
                START_TIMER == (*it).second ||
                BEFORE_CHECK == (*it).second)
            {
                setBenchFail((*it).first);
            }
        }
 
        return true;
    }
    else
    {
        return false;
    }
}

bool AutoTuneMemo::loadMemoFile(const set< string >& fileName)
{
    bool rc = true;

    for (set< string >::const_iterator
         it = fileName.begin(); it != fileName.end(); it++)
    {
        rc = rc && loadMemoFile(*it);
    }

    return rc;
}

bool AutoTuneMemo::saveMemoFile(const string& fileName)
{
    ofstream ofs(fileName.c_str());

    if (ofs.is_open())
    {
        // entry states
        for (map< string, long >::const_iterator
             it = _workingState.begin(); it != _workingState.end(); it++)
        {
            const string kernelName = (*it).first;
            const long entryState = (*it).second;

            ofs << _deviceNumber << " "
                << kernelName << " "
                << entryState
                << endl;
        }

        // benchmark times
        for (map< string, vector< size_t > >::const_iterator
             it = _benchTime.begin(); it != _benchTime.end(); it++)
        {
            const string kernelName = (*it).first;

            for (vector< size_t >::const_iterator
                 jt = (*it).second.begin(); jt != (*it).second.end(); jt++)
            {
                const size_t microseconds = *jt;

                ofs << _deviceNumber << " "
                    << kernelName << " "
                    << microseconds
                    << endl;
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool AutoTuneMemo::printSource(const string& kernelName)
{
    return addEntry(kernelName, PRINT_SOURCE);
}

bool AutoTuneMemo::compileKernel(const string& kernelName)
{
    return addEntry(kernelName, COMPILE_KERNEL);
}

bool AutoTuneMemo::setKernelArgs(const string& kernelName)
{
    return addEntry(kernelName, SET_KERNEL_ARGS);
}

bool AutoTuneMemo::startTimer(const string& kernelName)
{
    return addEntry(kernelName, START_TIMER);
}

bool AutoTuneMemo::stopTimer(const string& kernelName)
{
    return addEntry(kernelName, STOP_TIMER);
}

bool AutoTuneMemo::beforeCheck(const string& kernelName)
{
    return addEntry(kernelName, BEFORE_CHECK);
}

bool AutoTuneMemo::afterCheck(const string& kernelName)
{
    return addEntry(kernelName, AFTER_CHECK);
}

bool AutoTuneMemo::setBenchTime(const string& kernelName,
                                const size_t microseconds)
{
    return addEntry(kernelName, GOOD_OUTPUT) &&
           addEntry(kernelName, microseconds);
}

bool AutoTuneMemo::setBenchFail(const string& kernelName)
{
    return addEntry(kernelName, BAD_OUTPUT);
}

bool AutoTuneMemo::isKernelGood(const string& kernelName)
{
    return 0 == _badKernels.count(kernelName) &&
           0 != _benchTime.count(kernelName);
}

bool AutoTuneMemo::isKernelBad(const string& kernelName)
{
    return 0 != _badKernels.count(kernelName);
}

size_t AutoTuneMemo::getBenchTime(const string& kernelName,
                                  const size_t trialNumber)
{
    if (isKernelGood(kernelName) &&
        trialNumber < _benchTime[kernelName].size())
    {
        return _benchTime[kernelName][trialNumber];
    }
    else
    {
        return -1;
    }
}

}; // namespace chai_internal
