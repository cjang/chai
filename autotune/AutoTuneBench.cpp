// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

#include "AutoTuneBench.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// autotuning controller

size_t AutoTuneBench::runTime(const size_t trialNumber,
                              KernelBase& kernelGen,
                              const bool includeSendDataToDevice,
                              const bool includeReadDataFromDevice,
                              ostream* os)
{
    // kernel name is unique identifier as it includes the parameters
    const string kernelName = kernelGen.kernelName();

    if (os) *os << "kernelName is " << kernelName << endl;

    // check for known bad kernels
    if (_journal.isKernelBad(kernelName))
    {
        // skip this kernel
        return -1;
    }

    if (os) *os << "kernel not bad" << endl;

    // check memo before benchmarking
    const size_t memoTime = _journal.getBenchTime(kernelName,
                                                  trialNumber);
    if (-1 != memoTime)
    {
        return memoTime;
    }

    // start compilation of the kernel
    _journal.printSource(kernelName);

    // kernel source text
    stringstream ss;
    ss << kernelGen;
    const string kernelSource = ss.str();

    if (os) *os << kernelSource << endl;

    // next step is compiling
    _journal.compileKernel(kernelName);

    // kernel object, this compiles the source text
    OCLkernel ckernel(_cdev,
                      kernelName,
                      kernelSource);

    // check for failure
    if (! ckernel.isOk())
    {
        _journal.setBenchFail(kernelName);

        if (os) *os << "error compiling kernel" << endl;

        // kernel compile failed
        return -1;
    }

    if (os) *os << "kernel compile ok" << endl;

    // set arguments to the kernel
    _journal.setKernelArgs(kernelName);

    // set kernel arguments
    if (! kernelGen.setArgs(_cdev, ckernel))
    {
        _journal.setBenchFail(kernelName);

        if (os) *os << "error setting kernel arguments" << endl;

        // failed to set kernel arguments
        return -1;
    }

    if (os) *os << "set kernel arguments ok" << endl;

    // send input data, exclude PCIe bus transfer cost
    if (! includeSendDataToDevice)
    {
        if (! kernelGen.syncInput())
        {
            _journal.setBenchFail(kernelName);

            if (os) *os << "error with data transfer to device" << endl;

            // data transfer to device failed
            return -1;
        }

        if (os) *os << "send data to device ok" << endl;
    }

    // index space dimensions
    const vector< size_t > globalDims = kernelGen.globalWorkItems();
    const vector< size_t > localDims = kernelGen.localWorkItems();

    // begin benchmark
    _journal.startTimer(kernelName);

    // start timer
    struct timeval start_time;
    if (-1 == gettimeofday(&start_time, 0))
    {
        if (os) *os << "error starting timer" << endl;

        // could not start timer
        return -1;
    }

    // send input data, include PCIe bus transfer cost
    if (includeSendDataToDevice)
    {
        if (! kernelGen.syncInput())
        {
            _journal.setBenchFail(kernelName);

            if (os) *os << "error with data transfer to device" << endl;

            // data transfer to device failed
            return -1;
        }

        if (os) *os << "send data to device ok" << endl;
    }

    // set work index space
    for (size_t j = 0; j < globalDims.size(); j++)
    {
        ckernel << OCLWorkIndex(globalDims[j], localDims[j]);

        if (! ckernel.statusOp())
        {
            _journal.setBenchFail(kernelName);

            if (os) *os << "error setting index space" << endl;

            // failed to set index space
            return -1;
        }
    }

    // enqueue kernel
    _cdev << ckernel;

    // check for enqueue failure
    if (! _cdev.statusOp())
    {
        _journal.setBenchFail(kernelName);

        if (os) *os << "error enqueuing kernel" << endl;

        // failed to enqueue
        return -1;
    }

    // wait for kernel to finish
    _cdev << FLUSH;

    // check for waiting failure
    if (! _cdev.statusOp())
    {
        _journal.setBenchFail(kernelName);

        if (os) *os << "error waiting for enqueued kernels" << endl;

        // error waiting for kernels
        return -1;
    }

    // read back output data, include PCIe bus transfer cost
    if (includeReadDataFromDevice)
    {
        if (! kernelGen.syncOutput())
        {
            _journal.setBenchFail(kernelName);

            if (os) *os << "error with data transfer from device" << endl;

            // data transfer from device failed
            return -1;
        }

        if (os) *os << "read data from device ok" << endl;
    }

    // stop timer
    struct timeval stop_time;
    if (-1 == gettimeofday(&stop_time, 0))
    {
        if (os) *os << "error stopping timer" << endl;

        // could not stop timer
        return -1;
    }

    // benchmark finished
    _journal.stopTimer(kernelName);

    // read back output data, exclude PCIe bus transfer cost
    if (! includeReadDataFromDevice)
    {
        if (! kernelGen.syncOutput())
        {
            _journal.setBenchFail(kernelName);

            if (os) *os << "error with data transfer from device" << endl;

            // data transfer from device failed
            return -1;
        }

        if (os) *os << "read data from device ok" << endl;
    }

    // calculate elapsed time in microseconds
    const size_t elapsed_time
        = 1000 * 1000 * ( stop_time.tv_sec - start_time.tv_sec )
              + stop_time.tv_usec
              + (1000 * 1000 - start_time.tv_usec)
              - 1000 * 1000;

    // check output results
    if (kernelGen.checkOutput())
    {
        _journal.setBenchTime(kernelName,
                              elapsed_time);

        return elapsed_time;
    }
    else
    {
        _journal.setBenchFail(kernelName);

        return -1;
    }
}

AutoTuneBench::AutoTuneBench(OCLdevice& cdev,
                             AutoTuneMemo& journal)
    : _cdev(cdev),
      _journal(journal) { }

size_t AutoTuneBench::runTime(const size_t trialNumber,
                              KernelBase& kernelGen,
                              const bool includeSendDataToDevice,
                              const bool includeReadDataFromDevice)
{
    return runTime(trialNumber,
                   kernelGen,
                   includeSendDataToDevice,
                   includeReadDataFromDevice,
                   NULL);
}

size_t AutoTuneBench::runTime(const size_t trialNumber,
                              KernelBase& kernelGen,
                              const bool includeSendDataToDevice,
                              const bool includeReadDataFromDevice,
                              ostream& os)
{
    return runTime(trialNumber,
                   kernelGen,
                   includeSendDataToDevice,
                   includeReadDataFromDevice,
                   &os);
}

}; // namespace chai_internal
