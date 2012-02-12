// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <string>

#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "OCLdevice.hpp"
#include "OCLhacks.hpp"
#include "StandardEM.hpp"
#include "Watchdog.hpp"
#include "chai/ParseArgs.hpp"

using namespace chai_internal;
using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    if (1 == argc)
    {
        cerr << "usage: " << argv[0]
             << " -f configspec -p SDD -m M [-n N] [-t T]"
             << " [-c batching] [-g|-G] [-v 042|x1x|777] [-s] [-r]"
             << endl;
        exit(1);
    }

    ParseArgs pargs(argc, argv);

    // initialize OpenCL
    OCLinit cinit;
    if (! pargs.getOpt(cinit))
    {
        cerr << "error: parsing arguments" << endl;
        exit(1);
    }
    const size_t deviceIdx = pargs.getDeviceIndex();
    OCLdevice cdev(cinit, deviceIdx);

    const size_t numSearch = OCLhacks::singleton().searchTrials(deviceIdx);
    const size_t numTiming = OCLhacks::singleton().timingTrials(deviceIdx);

    StandardEM stdEM(cdev);
    stdEM.setJournalFile(OCLhacks::singleton().journalFile(deviceIdx));
    stdEM.setNumberTrials(numSearch, numTiming);
    stdEM.setDebug(cout);
    pargs.setOpt(stdEM);

    // ATI Evergreen matrix multiply
    Evergreen::MatmulMV matvec(deviceIdx);
    matvec.setPacking(1);     // default
    matvec.setGeneral(false); // default
    pargs.setOpt(matvec);

    // detect OpenCL compiler hangs and exit the process
    const double watchSecs = OCLhacks::singleton().watchdogSecs(deviceIdx);
    Watchdog::singleton().enableDog(watchSecs);

    // vector length combinations
    size_t startA, startB, startC, stopA, stopB, stopC;
    pargs.setOpt(matvec, startA, startB, startC, stopA, stopB, stopC);

    for (size_t vlenA = startA; vlenA <= stopA; vlenA++)
    for (size_t vlenB = startB; vlenB <= stopB; vlenB++)
    for (size_t vlenC = startC; vlenC <= stopC; vlenC++)
    {
        Watchdog::singleton().resetDog();

        if (3 == vlenA || 3 == vlenB || 3 == vlenC)
            continue;

        matvec.activeOptimizeWorkGroup();
        matvec.activeOptimizeInnerBlocking();
        matvec.doNotOptimizeVectorLength();
        matvec.inactiveOptimizeExtraParam();
        matvec.setVectorLength(vlenA, vlenB, vlenC);
        matvec.setExtraParam(0);

        // find optimal kernel
        matvec.sentinelCheck();
        if (! stdEM.searchLoop(matvec))
        {
            cout << "FAILED no good kernel found" << endl;
            continue;
        }

        // paranoid check
        const double threshold = OCLhacks::singleton().paranoidError(deviceIdx);
        if (threshold < 0)
        {
            // if threshold is negative, check will always fail
            cout << "SKIPPING paranoid check" << endl;
        }
        else
        {
            stdEM.beforeCheck(matvec);

            matvec.paranoidCheck(threshold);
            if (-1 == stdEM.timingLoop(matvec))
            {
                cout << "FAILED paranoid check" << endl;
            }
            else
            {
                matvec.noCheck();
                const double GFLOPS = stdEM.timingLoop(matvec);
                if (-1 == GFLOPS)
                {
                    cout << "FAILED final timing loop" << endl;
                }
                else
                {
                    cout << endl
                         << matvec
                         << endl 
                         << "GFLOPS " << GFLOPS << endl;
                }
            }

            stdEM.afterCheck(matvec);
        }
    }

    Watchdog::singleton().disableDog();

    cout << "***FINISH***" << endl;

    exit(0);
}
