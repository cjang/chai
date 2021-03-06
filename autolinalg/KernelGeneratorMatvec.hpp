// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_GENERATOR_MATVEC_HPP_
#define _CHAI_KERNEL_GENERATOR_MATVEC_HPP_

#include <ostream>

#include "ArrayBuf.hpp"
#include "AutoParamsMatmul.hpp"
#include "GeneratorBaseMatvec.hpp"
#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// GEMV kernel generator

class KernelGeneratorMatvec : public GeneratorBaseMatvec,
                              protected AutoParamsMatmul::ParamInlineDim,
                              protected AutoParamsMatmul::ParamGlobalID
{
    ArrayBuf* _A;
    ArrayBuf* _B;
    ArrayBuf* _C;

    enum CheckOutputMode
    {
        NO_CHECK,
        SENTINEL_CHECK,
        PARANOID_CHECK
    };

    CheckOutputMode _checkOutputMode;
    bool            _checkOutputTouched;
    double          _paranoidLimit;

    // GEMV is alpha times AB plus beta times C
    double _alpha;
    double _beta;

    const size_t _deviceIndex;

    bool setArgsInternal(OCLkernel& ckernel,
                         ArrayBuf* A,
                         ArrayBuf* B,
                         ArrayBuf* C,
                         const double alpha,
                         const double beta);

public:
    KernelGeneratorMatvec(const size_t deviceIndex);
    ~KernelGeneratorMatvec(void);

    // validate calculation
    void noCheck(void);
    void sentinelCheck(void);
    void paranoidCheck(const double absDiffLimit);

    // (autotuning) allocate buffers and set kernel matrix arguments
    bool setArgs(OCLdevice& cdev,
                 OCLkernel& ckernel);

    // invoke kernel on real data
    bool setArgs(OCLkernel& ckernel,
                 ArrayBuf* A,
                 ArrayBuf* B,
                 ArrayBuf* C,
                 const double alpha = 1,
                 const double beta = 0);

    void clearArgs(void);

    // data transfer
    bool syncInput(void);
    bool syncOutput(void);

    // check output, sometimes bad kernels do nothing
    bool checkOutput(void);

    // prints the kernel source
    std::ostream& print(std::ostream&) const;
};

}; // namespace chai_internal

#endif
