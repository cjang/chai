// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SOURCE_KERNEL_HPP_
#define _CHAI_SOURCE_KERNEL_HPP_

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "KernelUnit.hpp"
#include "Variable.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// compute kernel from source code

class SourceKernel : public KernelUnit
{
    const std::string                _functionName;
    const std::vector< std::string > _sourceText;
    const uint64_t                   _sourceHashCode;

    std::vector< Variable* >         _arguments;
    std::map< Variable*, uint32_t >  _varTrace;
    std::map< uint32_t, Variable* >  _traceVar;

public:
    SourceKernel(const std::string& functionName,
                 const std::vector< std::string >& sourceText,
                 const uint64_t                    sourceHashCode);
    ~SourceKernel(void);

    std::string functionName(void) const;

    const std::vector< Variable* >& arguments(void) const;

    void pushArgArray(const size_t PREC, const uint32_t varNum);
    void pushArgLocal(const size_t PREC, const size_t length);
    void pushArgScalar(const uint32_t a);
    void pushArgScalar(const int32_t a);
    void pushArgScalar(const float a);
    void pushArgScalar(const double a);

    uint32_t traceVar(Variable*);
    Variable* traceVar(const uint32_t);

    const AstVariable* splitVar(Variable*);
    Variable* splitVar(const AstVariable*);

    void print(std::vector< std::string >&);
};

}; // namespace chai_internal

#endif
