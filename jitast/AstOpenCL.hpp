// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_OPENCL_HPP_
#define _CHAI_AST_OPENCL_HPP_

#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

#include "BaseAst.hpp"
#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// inline OpenCL

class AstOpenCL : public BaseAst
{
    const std::vector< std::string > _programSource;
    const uint64_t                   _programHashCode;
    const std::string                _kernelName;

    enum ArgumentKind
    {
        ARRAY_VARIABLE,
        LOCAL_MEM,
        SCALAR
    };

    std::map< size_t, ArgumentKind > _argKind;
    std::map< size_t, size_t >       _argPrec;

    union ArgumentUnion
    {
        size_t   s;
        uint32_t u;
        int32_t  i;
        float    f;
        double   d;
    };

    std::map< size_t, ArgumentUnion > _argValue;

    std::vector< size_t > _workGlobal;
    std::vector< size_t > _workLocal;

public:
    AstOpenCL(const std::vector< std::string >& programSource,
              const uint64_t programHashCode,
              const std::string& kernelName);

    ~AstOpenCL(void);

    const std::vector< std::string >& programSource(void) const;
    uint64_t programHashCode(void) const;
    const std::string& kernelName(void) const;

    size_t getWorkDim(void) const;
    size_t getGlobalWorkDim(const size_t i) const;
    size_t getLocalWorkDim(const size_t i) const;

    void setWorkSpace(const size_t global0,
                      const size_t local0);

    void setWorkSpace(const size_t global0,
                      const size_t global1,
                      const size_t local0,
                      const size_t local1);

    void setArgArray(const size_t i, const size_t PREC, const uint32_t v);
    void setArgLocal(const size_t i, const size_t PREC, const size_t n);
    void setArgScalar(const size_t i, const size_t PREC, const uint32_t a);
    void setArgScalar(const size_t i, const size_t PREC, const int32_t a);
    void setArgScalar(const size_t i, const size_t PREC, const float a);
    void setArgScalar(const size_t i, const size_t PREC, const double a);

    size_t getNumArgs(void) const;

    bool isArgArray(const size_t i);
    bool isArgLocal(const size_t i);
    bool isArgScalar(const size_t i);

    size_t getArgPrec(const size_t i);

    size_t getArgSizeT(const size_t i);
    uint32_t getArgUInt32(const size_t i);
    int32_t getArgInt32(const size_t i);
    float getArgFloat(const size_t i);
    double getArgDouble(const size_t i);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
