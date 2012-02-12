// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_BASE_HPP_
#define _CHAI_KERNEL_BASE_HPP_

#include <set>
#include <string>
#include <vector>

#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// base interface for kernel generators

class KernelBase
{
public:
    virtual ~KernelBase(void);

    // number of floating point operations
    virtual size_t numberFlops(void) const = 0;

    // kernel source is parameterized
    virtual void toggleParamMarks(void) = 0;
    virtual bool getParams(std::set< std::vector<size_t> >& inParams) = 0;
    virtual void setParams(const std::vector<size_t>& inParams) = 0;

    // work item index space
    virtual std::vector<size_t> globalWorkItems(void) const = 0;
    virtual std::vector<size_t> localWorkItems(void) const = 0;

    // allocate buffers and set kernel matrix arguments
    virtual bool setArgs(OCLdevice& cdev,
                         OCLkernel& ckernel) = 0;

    // data transfer
    virtual bool syncInput(void) = 0;
    virtual bool syncOutput(void) = 0;

    // check output, sometimes bad kernels do nothing
    virtual bool checkOutput(void) = 0;

    // return the kernel name
    virtual std::string kernelName(void) const = 0;

    // prints the kernel source
    virtual std::ostream& print(std::ostream&) const = 0;
};

std::ostream& operator<< (std::ostream& os,
                          const KernelBase& kernelGen);

}; // namespace chai_internal

#endif
