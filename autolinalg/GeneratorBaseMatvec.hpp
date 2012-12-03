// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_GENERATOR_BASE_MATVEC_HPP_
#define _CHAI_GENERATOR_BASE_MATVEC_HPP_

#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "CodeVars.hpp"
#include "KernelBaseMatmul.hpp"

namespace chai_internal {

////////////////////////////////////////
// matrix vector multiply

class GeneratorBaseMatvec : public KernelBaseMatmul
{
protected:
    virtual ~GeneratorBaseMatvec(void);

    // inner product accumulation
    std::string assignMAD(
                    const std::vector< PrivateVar >& accum,
                    const std::vector< PrivateVar >& valueA,
                    const std::vector< PrivateVar >& valueB,
                    const size_t j,         // output row
                    const size_t l ) const; // inner product column

public:
    std::string kernelForename(void) const;

    bool validParams(void) const;
    bool getParams(std::vector<size_t>& params) const;
    std::string exogenousKey(void) const;

    void toggleParamMarks(void);                               // pure virtual
    bool getParams(std::set< std::vector<size_t> >& paramSet); // pure virtual
    void setParams(const std::vector<size_t>& params);         // pure virtual
    std::string kernelName(void) const;                        // pure virtual
    std::vector<size_t> globalWorkItems(void) const;           // pure virtual
    std::vector<size_t> localWorkItems(void) const;            // pure virtual
    size_t numberFlops(void) const;                            // pure virtual
};

}; // namespace chai_internal

#endif
