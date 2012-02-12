// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AUTO_TUNE_EM_HPP_
#define _CHAI_AUTO_TUNE_EM_HPP_

#include <map>
#include <ostream>
#include <vector>

#include "AutoTuneBench.hpp"
#include "KernelBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// expectation maximization auto-tuning

class AutoTuneEM
{
    AutoTuneBench& _bench;
    KernelBase&    _kernelGen;
    bool           _inputDataTransferCost;
    bool           _outputDataTransferCost;
    std::ostream*  _os;
    size_t         _numberTrials;

public:
    AutoTuneEM(AutoTuneBench& bench,
               KernelBase& kernelGen);

    void inputDataTransfer(const bool includeCost);
    void outputDataTransfer(const bool includeCost);

    void debugStream(std::ostream& os);
    void debugStream(void);

    void setNumberTrials(const size_t n);

    // one EM step
    std::map< std::vector< size_t >, size_t >
        optimizeStep( std::vector< size_t >& best );

    // alternate E and M steps by toggling parameter marks
    void toggleStep(void);
};

}; // namespace chai_internal

#endif
