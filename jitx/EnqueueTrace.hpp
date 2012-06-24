// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ENQUEUE_TRACE_HPP_
#define _CHAI_ENQUEUE_TRACE_HPP_

#include <ostream>
#include <sstream>
#include <stack>
#include <string>

#include "Function.hpp"
#include "JustInTimeMemo.hpp"
#include "MemManager.hpp"
#include "PrintTrace.hpp"
#include "StandardEM.hpp"
#include "Subscript.hpp"
#include "VectorTrace.hpp"
#include "VisitAst.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace

class EnqueueTrace : public VisitStmt,
                     public VisitAst
{
    VectorTrace&    _vt;
    MemManager&     _memMgr;
    StandardEM&     _stdEM;
    JustInTimeMemo& _jitMemo;
    PrintTrace*     _printer;

    // visiting state
    StmtIdSpace* _xid;
    bool         _failure;
    Function*    _func;
    size_t       _minVectorLength; // over variables in current StmtIdSpace
    size_t       _maxVectorLength; // over variables in current StmtIdSpace
    size_t       _lhsVectorLength;   // current statement
    size_t       _smallVectorLength; // current statement
    size_t       _largeVectorLength; // current statement
    size_t       _lhsPrecision;      // current statement
    std::stack< Subscript* > _subs;
    std::stringstream        _rhsStmt;
    std::string              _component;
    bool         _scalarToScalar; // hack for scalar to scalar assignment
    std::stack< size_t >     _repeatIdx;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

    // find minimum and maximum of variable vector lengths for xid and jitMemo
    void boundVectorLength(void);

    size_t getVectorLength(const uint32_t varNum);
    size_t getVectorLength(const AstVariable* varPtr);

public:
    EnqueueTrace(VectorTrace&,
                 MemManager&,
                 StandardEM&,
                 JustInTimeMemo&);

    ~EnqueueTrace(void);

    bool isOk(void) const;

    void visit(AstAccum&);
    void visit(AstArrayMem&);
    void visit(AstCond&);
    void visit(AstConvert&);
    void visit(AstDotprod&);
    void visit(AstExtension&);
    void visit(AstFun1&);
    void visit(AstFun2&);
    void visit(AstFun3&);
    void visit(AstGather&);
    void visit(AstIdxdata&);
    void visit(AstLitdata&);
    void visit(AstMakedata&);
    void visit(AstMatmulMM&);
    void visit(AstMatmulMV&);
    void visit(AstMatmulVM&);
    void visit(AstMatmulVV&);
    void visit(AstOpenCL&);
    void visit(AstReadout&);
    void visit(AstRNGnormal&);
    void visit(AstRNGuniform&);
    void visit(AstScalar&);
    void visit(AstTranspose&);
    void visit(AstVariable&);

    void visit(StmtBarrier&);
    void visit(StmtCompound&);
    void visit(StmtCreateData&);
    void visit(StmtExtension&);
    void visit(StmtExtensionAuto&);
    void visit(StmtGatherAuto&);
    void visit(StmtIdSpace&);
    void visit(StmtIndex&);
    void visit(StmtLiteral&);
    void visit(StmtMatmul&);
    void visit(StmtMatmulAuto&);
    void visit(StmtOpenCL&);
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);
};

}; // namespace chai_internal

#endif
