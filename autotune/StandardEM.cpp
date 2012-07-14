// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "AutoTuneEM.hpp"
#include "StandardEM.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// standard application use of EM

StandardEM::StandardEM(OCLdevice& cdev)
    : _cdev(cdev),
      _searchJournal(),
      _searchBench( AutoTuneBench(cdev, _searchJournal) ),
      _numberSearchTrials(1),
      _numberTimingTrials(1),
      _includeSendDataToDevice(false),
      _includeReadDataFromDevice(false),
      _os(NULL)
{
}

void StandardEM::setJournalFile(const string& journalFile)
{
    _searchJournal.loadMemoFile(journalFile);
    _searchJournal.insertJournalFile(journalFile);
}

void StandardEM::setJournalFile(const set< string >& journalFileSet)
{
    for (set< string >::const_iterator
         it = journalFileSet.begin(); it != journalFileSet.end(); it++)
    {
        setJournalFile(*it);
    }
}

void StandardEM::setDebug(ostream& os)
{
    _os = &os;
}

void StandardEM::setDebug(void)
{
    _os = NULL;
}

void StandardEM::setNumberTrials(const size_t searchTrials,
                                 const size_t timingTrials)
{
    _numberSearchTrials = searchTrials;
    _numberTimingTrials = timingTrials;
}

void StandardEM::setIncludeSendDataToDevice(const bool setting)
{
    _includeSendDataToDevice = setting;
}

void StandardEM::setIncludeReadDataFromDevice(const bool setting)
{
    _includeReadDataFromDevice = setting;
}

bool StandardEM::searchLoop(KernelBase& kernelGen)
{
    AutoTuneEM searchEM(_searchBench, kernelGen);
    searchEM.setNumberTrials(_numberSearchTrials);
    if (_os)
        searchEM.debugStream(*_os);
    if (_includeSendDataToDevice)
        searchEM.inputDataTransfer(_includeSendDataToDevice);
    if (_includeReadDataFromDevice)
        searchEM.outputDataTransfer(_includeReadDataFromDevice);

    vector< size_t > lastBest, currentBest;
    lastBest.push_back(0);

    size_t currentBestTime;

    // expectation-maxization search for fixed point
    while (lastBest != currentBest)
    {
        lastBest = currentBest;

        currentBest.clear();

        map< vector< size_t >, size_t >
            paramMap = searchEM.optimizeStep( currentBest );

        if (currentBest.empty())
            return false; // failed to find any good kernel specializations

        searchEM.toggleStep();

        currentBestTime = paramMap[ currentBest ];
    }

    // print optimal parameters found
    if (_os)
    {
        *_os << "FINAL";

        for (vector< size_t >::const_iterator
             it = currentBest.begin(); it != currentBest.end(); it++)
        {
            *_os << " " << *it;
        }

        *_os << " " << currentBestTime << endl;
    }

    // set optimal parameters in kernel generator
    kernelGen.setParams(currentBest);

    return true;
}

double StandardEM::timingLoop(KernelBase& kernelGen)
{
    AutoTuneMemo journal(_cdev.deviceIndex());
    AutoTuneBench bench(_cdev, journal);

    size_t totalTime = 0;

    for (size_t i = 0; i < _numberTimingTrials; i++)
    {
        const size_t microsecs = bench.runTime(i,
                                               kernelGen,
                                               _includeSendDataToDevice,
                                               _includeReadDataFromDevice);

        if (-1 == microsecs)
            return -1; // failure
        else
            totalTime += microsecs;
    }

    const double GFLOPS = static_cast<double>( _numberTimingTrials *
                                               kernelGen.numberFlops() )
                              / (1000.0f * totalTime);

    return GFLOPS; // success
}

void StandardEM::beforeCheck(const KernelBase& kernelGen)
{
    _searchJournal.beforeCheck(kernelGen.kernelName());
}

void StandardEM::afterCheck(const KernelBase& kernelGen)
{
    _searchJournal.afterCheck(kernelGen.kernelName());
}

}; // namespace chai_internal
