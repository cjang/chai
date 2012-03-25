// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_GROUP_TOGETHER_HPP_
#define _CHAI_GROUP_TOGETHER_HPP_

#include <map>
#include <set>
#include <vector>

#include "AstVariable.hpp"
#include "VisitStmt.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// index space compatibility

class GroupTogether : public VisitStmt
{
public:
    enum SpecialKernel
    {
        NOTHING_SPECIAL,

        // language extensions
        SPECIAL_EXTENSION,
        SPECIAL_EXTENSION_AUTO,

        // matrix multiplication, GEMM and GEMV
        SPECIAL_MATMUL,
        SPECIAL_MATMUL_AUTO
    };

private:
    class VariableDesc
    {
        const AstVariable* _variableObj;  // representative AST variable object
        bool _isArgument;   // function argument or private register
        bool _appearsOnLHS; // appears on LHS for at least one kernel
        bool _appearsOnRHS; // appears on RHS for at least one kernel

    public:
        VariableDesc(void);
        VariableDesc(const AstVariable* obj, const bool isArg);
        VariableDesc(const VariableDesc& other);

        VariableDesc& operator= (const VariableDesc& other);

        bool isArgument(void) const;
        bool isSameDataAcrossTraces(void) const;

        // access modifiers to individual kernels
        bool isReadOnly(const bool appearsOnLHS,
                        const bool appearsOnRHS) const;
        bool isWriteOnly(const bool appearsOnLHS,
                         const bool appearsOnRHS) const;
        bool isReadWrite(const bool appearsOnLHS,
                         const bool appearsOnRHS) const;

        bool getAppearsOnLHS(void) const;
        bool getAppearsOnRHS(void) const;
        void setAppearsOnLHS(void);
        void setAppearsOnRHS(void);

        // access modifiers over all kernels in a trace
        bool isReadOnly(void) const;
        bool isWriteOnly(void) const;
        bool isReadWrite(void) const;

        size_t width(void) const;
        size_t height(void) const;
        size_t precision(void) const;
    };

    // statement list
    std::vector< Stmt* > _togetherList;

    // trace/split variables are either function arguments or private
    std::map< uint32_t, VariableDesc >           _traceVarDesc;
    std::map< const AstVariable*, VariableDesc > _splitVarDesc;

    // function arguments
    std::set< uint32_t >           _traceArgs;
    std::set< const AstVariable* > _splitArgs;

    // RHS references to earlier LHS variables require synchronization barrier
    std::set< uint32_t >           _traceBarrier;
    std::set< const AstVariable* > _splitBarrier;

    // track variable sideness
    std::set< uint32_t >           _traceLHS;
    std::set< uint32_t >           _traceRHS;
    std::set< const AstVariable* > _splitLHS;
    std::set< const AstVariable* > _splitRHS;

    // track variables that require transposed array subscripts
    std::set< uint32_t >           _traceTransposed;
    std::set< const AstVariable* > _splitTransposed;

    // track variables that require gathered array subscripts
    std::set< uint32_t >           _traceGathered;
    std::set< const AstVariable* > _splitGathered;

    // track variable lexical scope within kernel
    std::set< uint32_t > _constructorLHS;
    std::set< uint32_t > _destructorLHS;

    // stream dimensions
    size_t _streamW;
    size_t _streamH;

    size_t _lexicalDepth;

    bool          _prevIsExtension;
    bool          _prevIsMatmul;
    bool          _prevIsReduction;
    SpecialKernel _specialK;

    bool          _reductionIsSpecial;

    bool isExtension(void);
    bool isMatmul(void);
    bool isReduction(void);
    bool isSpecial(void);
    bool isEmpty(void) const;
    bool isPragmas(void) const;
    bool isCode(void) const;

    void boundIndex(const size_t w, const size_t h);
    void boundIndex(const GroupTogether&);
    void boundIndex(const Stmt&);
    void setIndex(const StmtReadData&);

    void pushList(Stmt&);
    void pushList(Stmt&, GroupTogether&);

    void insertVariable(const AstVariable*,
                        const bool appearsOnLHS,
                        const bool appearsOnRHS);

    void barrierVariable(AstVariable* lhsVar);
    bool checkBarrier(const std::set< AstVariable* >& rhsVars,
                      std::set< uint32_t >& traceMatch,
                      std::set< const AstVariable* >& splitMatch);

public:
    GroupTogether(void);
    GroupTogether(const GroupTogether&);

    void clear(void);

    const std::vector< Stmt* >& stuffInside(void) const;

    const std::set< uint32_t >& traceArguments(void) const;
    const std::set< const AstVariable* >& splitArguments(void) const;

    const std::set< uint32_t >& traceLHS(void) const;
    const std::set< uint32_t >& traceRHS(void) const;
    const std::set< const AstVariable* >& splitLHS(void) const;
    const std::set< const AstVariable* >& splitRHS(void) const;

    const std::set< uint32_t >& traceTransposed(void) const;
    const std::set< const AstVariable* >& splitTransposed(void) const;

    const std::set< uint32_t >& traceGathered(void) const;
    const std::set< const AstVariable* >& splitGathered(void) const;

    // each kernel has different sideness to variable arguments
    bool isReadOnly(const uint32_t,
                    const bool appearsOnLHS,
                    const bool appearsOnRHS) const;
    bool isReadOnly(const AstVariable*,
                    const bool appearsOnLHS,
                    const bool appearsOnRHS) const;
    bool isWriteOnly(const uint32_t,
                     const bool appearsOnLHS,
                     const bool appearsOnRHS) const;
    bool isWriteOnly(const AstVariable*,
                     const bool appearsOnLHS,
                     const bool appearsOnRHS) const;
    bool isReadWrite(const uint32_t,
                     const bool appearsOnLHS,
                     const bool appearsOnRHS) const;
    bool isReadWrite(const AstVariable*,
                     const bool appearsOnLHS,
                     const bool appearsOnRHS) const;

    // variable sideness over all kernels in a trace
    bool isReadOnly(const uint32_t) const;
    bool isReadOnly(const AstVariable*) const;
    bool isWriteOnly(const uint32_t) const;
    bool isWriteOnly(const AstVariable*) const;
    bool isReadWrite(const uint32_t) const;
    bool isReadWrite(const AstVariable*) const;

    size_t width(const uint32_t) const;
    size_t width(const AstVariable*) const;
    size_t height(const uint32_t) const;
    size_t height(const AstVariable*) const;
    size_t precision(const uint32_t) const;
    size_t precision(const AstVariable*) const;

    // variables constructed and destructed within the kernel
    std::set< uint32_t > internalVars(void) const;

    size_t streamW(void) const;
    size_t streamH(void) const;

    bool isPush(Stmt*);
    SpecialKernel specialK(void) const;

    void reductionIsSpecial(const bool);

    void visit(StmtBarrier&);
    void visit(StmtCompound&);
    void visit(StmtCreateData&);
    void visit(StmtExtension&);
    void visit(StmtExtensionAuto&);
    void visit(StmtIdSpace&);
    void visit(StmtIndex&);
    void visit(StmtLiteral&);
    void visit(StmtMatmul&);
    void visit(StmtMatmulAuto&);
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtRNGrand&);
    void visit(StmtRNGseed&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);
};

}; // namespace chai_internal

#endif
