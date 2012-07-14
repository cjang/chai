// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ENQUEUE_TRACE_HPP_
#define _CHAI_ENQUEUE_TRACE_HPP_

#include <ostream>
#include <sstream>
#include <stack>
#include <string>

#include "Function.hpp"
#include "JustInTimeMemo.hpp"
#include "MemTrans.hpp"
#include "PrintTrace.hpp"
#include "StandardEM.hpp"
#include "Subscript.hpp"
#include "VectorTrace.hpp"
#include "VisitAst.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace

class Enqueue : public VisitStmt,
                public VisitAst
{
    VectorTrace&    _vt;
    MemTrans&       _memMgr;
    StandardEM&     _stdEM;
    JustInTimeMemo& _jitMemo;
    PrintTrace*     _printer;

    // visiting state
    StmtIdSpace* _xid;
    bool         _failure;
    Function*    _func;
    size_t       _minVecLength;   // over variables in current StmtIdSpace
    size_t       _maxVecLength;   // over variables in current StmtIdSpace
    size_t       _enqueueWidth;   // over current StmtIdSpace kernel
    size_t       _enqueueHeight;  // over current StmtIdSpace kernel
    size_t       _lhsVecLength;   // current statement
    size_t       _smallVecLength; // current statement
    size_t       _largeVecLength; // current statement
    size_t       _lhsPrec;        // current statement
    std::stack< Subscript* > _subs;
    std::stringstream        _rhsStmt;
    std::string              _component;
    bool         _scalarToScalar; // hack for scalar to scalar assignment
    std::stack< size_t >     _repeatIdx;

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

    // find minimum and maximum of variable vector lengths for xid and jitMemo
    void boundVecLength(void);

    size_t getVecLength(const uint32_t varNum);
    size_t getVecLength(const AstVariable* varPtr);

public:
    Enqueue(VectorTrace&,
            MemTrans&,
            StandardEM&,
            JustInTimeMemo&);

    ~Enqueue(void);

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
