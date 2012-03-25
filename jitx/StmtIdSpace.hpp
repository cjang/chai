// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_ID_SPACE_HPP_
#define _CHAI_STMT_ID_SPACE_HPP_

#include <map>
#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include "AstVariable.hpp"
#include "VectorTrace.hpp"
#include "Stmt.hpp"
#include "GroupTogether.hpp"

namespace chai_internal {

////////////////////////////////////////
// like a compound statement except
// with common index space

class StmtIdSpace : public Stmt
{
public:
    enum SpecialKernel
    {
        NOTHING_SPECIAL,
        SPECIAL_MATMUL,
        SPECIAL_MATMUL_AUTO
    };

private:
    const GroupTogether& _together;

    std::vector< Stmt* > _stmts;

    // trace/split variables
    std::set< uint32_t >                 _traceArgs;
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

    // index space dimensions
    const size_t _streamW;
    const size_t _streamH;

    // variable number -> read_scalar, read1, read2
    std::map< uint32_t, size_t > _traceReadoutDim;

    // these variables will have registers
    std::set< uint32_t > _traceUseRegister;
    std::set< uint32_t > _traceUseRegisterWriteBack;

    const uint64_t _hashCode;
    const size_t   _numTraces;
    const size_t   _kernelNumber;
    VectorTrace&   _vt;

    const SpecialKernel _specialK;

    SpecialKernel convertSpecialK(const GroupTogether::SpecialKernel) const;

public:
    StmtIdSpace(const GroupTogether& together,
                const size_t kernelNumber,
                VectorTrace& vt);

    ~StmtIdSpace(void);

    void setReadoutDim(const std::map< uint32_t, size_t >&);
    size_t getReadoutDim(const uint32_t);

    const std::set< uint32_t >& traceUseRegister(void) const;
    const std::set< uint32_t >& traceUseRegisterWriteBack(void) const;
    void traceUseRegister(const std::set< uint32_t >& varsUseRegisters,
                          const std::set< uint32_t >& varsWriteBack);

    void removeTraceArg(const uint32_t varNum);
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

    size_t streamW(void) const;
    size_t streamH(void) const;

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;
    size_t kernelNumber(void) const;

    VectorTrace& getVectorTrace(void);

    SpecialKernel specialK(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);

    const std::vector< Stmt* >& stuffInside(void) const;
    void replaceStuff(const std::vector< Stmt* >&);
};

}; // namespace chai_internal

#endif
