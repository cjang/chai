// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_ID_SPACE_HPP_
#define _CHAI_X_STMT_ID_SPACE_HPP_

#include <map>
#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include "AstVariable.hpp"
#include "VectorTrace.hpp"
#include "XStmt.hpp"
#include "XStmtTogether.hpp"

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

class XStmtIdSpace : public XStmt
{
public:
    enum SpecialKernel
    {
        NOTHING_SPECIAL,
        SPECIAL_MATMUL,
        SPECIAL_MATMUL_AUTO
    };

private:
    const XStmtTogether& _together;

    const std::vector< XStmt* > _stmts;

    // trace/split variables
    const std::set< uint32_t >           _traceArgs;
    const std::set< const AstVariable* > _splitArgs;

    // track variable sideness
    const std::set< uint32_t >           _traceLHS;
    const std::set< uint32_t >           _traceRHS;
    const std::set< const AstVariable* > _splitLHS;
    const std::set< const AstVariable* > _splitRHS;

    // track transposed variables (vector length 1 for array subscripts)
    const std::set< uint32_t >           _traceTransposed;
    const std::set< const AstVariable* > _splitTransposed;

    // track gathered variables (vector length 1 for array subscripts)
    const std::set< uint32_t >           _traceGathered;
    const std::set< const AstVariable* > _splitGathered;

    // track variable precision
    const bool _prec32;
    const bool _prec64;

    // index space dimensions
    const size_t _streamW;
    const size_t _streamH;

    // variable number -> read_scalar, read1, read2
    std::map< uint32_t, size_t > _traceReadoutDim;

    const uint64_t _hashCode;
    const size_t   _numTraces;
    const size_t   _kernelNumber;
    VectorTrace&   _vt;

    const SpecialKernel _specialK;

    SpecialKernel convertSpecialK(const XStmtTogether::SpecialKernel) const;

public:
    XStmtIdSpace(const XStmtTogether& together,
                 const size_t kernelNumber,
                 VectorTrace& vt);

    ~XStmtIdSpace(void);

    void setReadoutDim(const std::map< uint32_t, size_t >&);
    size_t getReadoutDim(const uint32_t);

    const std::set< uint32_t >& traceArgs(void) const;
    const std::set< const AstVariable* >& splitArgs(void) const;

    bool anyTransposed(void) const;
    bool traceTransposed(const uint32_t varNum) const;
    bool splitTransposed(const AstVariable* varPtr) const;

    bool anyGathered(void) const;
    bool traceGathered(const uint32_t varNum) const;
    bool splitGathered(const AstVariable* varPtr) const;

    bool isReadOnly(const uint32_t) const;
    bool isReadOnly(const AstVariable*) const;
    bool isWriteOnly(const uint32_t) const;
    bool isWriteOnly(const AstVariable*) const;
    bool isReadWrite(const uint32_t) const;
    bool isReadWrite(const AstVariable*) const;

    size_t getWidth(const uint32_t) const;
    size_t getWidth(const AstVariable*) const;
    size_t getHeight(const uint32_t) const;
    size_t getHeight(const AstVariable*) const;
    size_t getPrecision(const uint32_t) const;
    size_t getPrecision(const AstVariable*) const;

    bool isMixedPrecision(void) const;
    bool isSinglePrecision(void) const;
    bool isDoublePrecision(void) const;

    size_t streamW(void) const;
    size_t streamH(void) const;

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;
    size_t kernelNumber(void) const;

    VectorTrace& getVectorTrace(void);

    SpecialKernel specialK(void) const;

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);

    const std::vector< XStmt* >& stuffInside(void) const;
};

}; // namespace chai_internal

#endif
