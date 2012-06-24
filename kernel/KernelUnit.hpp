// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_UNIT_HPP_
#define _CHAI_KERNEL_UNIT_HPP_

#include <stdint.h>
#include <string>
#include <vector>

#include "Variable.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// base class for schedulable kernels

class KernelUnit
{
public:
    virtual ~KernelUnit(void);

    virtual std::string functionName(void) const = 0;

    virtual const std::vector< Variable* >& arguments(void) const = 0;

    virtual uint32_t traceVar(Variable*) = 0;
    virtual Variable* traceVar(const uint32_t) = 0;

    virtual const AstVariable* splitVar(Variable*) = 0;
    virtual Variable* splitVar(const AstVariable*) = 0;

    virtual void print(std::vector< std::string >&) = 0;
};

}; // namespace chai_internal

#endif
