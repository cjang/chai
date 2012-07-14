// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "AutoTuneEM.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// expectation maximization auto-tuning

AutoTuneEM::AutoTuneEM(AutoTuneBench& bench,
                       KernelBase& kernelGen)
    : _bench(bench),
      _kernelGen(kernelGen),
      _inputDataTransferCost(false),
      _outputDataTransferCost(false),
      _os(NULL),
      _numberTrials(1) { }

void AutoTuneEM::inputDataTransfer(const bool includeCost)
{
    _inputDataTransferCost = includeCost;
}

void AutoTuneEM::outputDataTransfer(const bool includeCost)
{
    _outputDataTransferCost = includeCost;
}

void AutoTuneEM::debugStream(ostream& os)
{
    _os = &os;
}

void AutoTuneEM::debugStream(void)
{
    _os = NULL;
}

void AutoTuneEM::setNumberTrials(const size_t n)
{
    _numberTrials = n;
}

map< vector< size_t >, size_t > AutoTuneEM::optimizeStep(
                                                vector< size_t >& best )
{
    map< vector< size_t >, size_t > paramMap;

    vector< size_t > bestParams;
    size_t           bestTime = -1;

    set< vector< size_t > > paramSet;

    if (_kernelGen.getParams(paramSet))
    {
        for (set< vector< size_t > >::const_iterator
             it = paramSet.begin(); it != paramSet.end(); it++)
        {
            if (_os)
            {
                for (vector< size_t >::const_iterator
                     jt = (*it).begin(); jt != (*it).end(); jt++)
                {
                    *_os << *jt << " ";
                }
            }

            _kernelGen.setParams(*it);

            size_t cumulativeTime = 0;

            for (size_t trialNumber = 0;
                 trialNumber < _numberTrials;
                 trialNumber++)
            {
                const size_t microsecs = _bench.runTime(
                                                    trialNumber,
                                                    _kernelGen,
                                                    _inputDataTransferCost,
                                                    _outputDataTransferCost );

                if (-1 != microsecs)
                {
                    cumulativeTime += microsecs;
                }
                else
                {
                    cumulativeTime = -1;
                    break;
                }
            }

            if (_os)
            {
                if (-1 == cumulativeTime)
                    *_os << "FAIL";
                else
                    *_os << cumulativeTime;
            }

            if (cumulativeTime < bestTime)
            {
                bestParams = *it;
                bestTime = cumulativeTime;

                if (_os)
                    *_os << " BEST";
            }

            if (_os)
                *_os << endl;

            paramMap[*it] = cumulativeTime;
        }
    }

    // leave the kernel generator with the optimal parameters, if any
    if (! bestParams.empty())
        _kernelGen.setParams(best = bestParams);

    return paramMap;
}

void AutoTuneEM::toggleStep(void)
{
    _kernelGen.toggleParamMarks();
}

}; // namespace chai_internal
