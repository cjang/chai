// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_ENQUEUE_HPP_
#define _CHAI_X_STMT_ENQUEUE_HPP_

#include <ostream>
#include <sstream>
#include <stack>
#include <string>

#include "Function.hpp"
#include "JustInTimeMemo.hpp"
#include "MemManager.hpp"
#include "StandardEM.hpp"
#include "Subscript.hpp"
#include "VectorTrace.hpp"
#include "VisitAst.hpp"
#include "VisitXStmt.hpp"
#include "XStmtPrinter.hpp"

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace

class XStmtEnqueue : public VisitXStmt,
                     public VisitAst
{
    VectorTrace&    _vt;
    MemManager&     _memMgr;
    StandardEM&     _stdEM;
    JustInTimeMemo& _jitMemo;
    XStmtPrinter*   _printer;

    // visiting state
    XStmtIdSpace* _xid;
    bool          _failure;
    Function*     _func;
    size_t        _minVectorLength; // over variables in current XStmtIdSpace
    size_t        _maxVectorLength; // over variables in current XStmtIdSpace
    size_t        _lhsVectorLength;   // current statement
    size_t        _smallVectorLength; // current statement
    size_t        _largeVectorLength; // current statement
    std::stack< Subscript* > _subs;
    std::stringstream        _rhsStmt;
    std::string              _component;
    bool          _scalarToScalar; // hack for scalar to scalar assignment

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

    // find minimum and maximum of variable vector lengths for xid and jitMemo
    void boundVectorLength(void);

    size_t getVectorLength(const uint32_t varNum);
    size_t getVectorLength(const AstVariable* varPtr);

public:
    XStmtEnqueue(VectorTrace&,
                 MemManager&,
                 StandardEM&,
                 JustInTimeMemo&);

    ~XStmtEnqueue(void);

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
    void visit(AstReadout&);
    void visit(AstRNGnormal&);
    void visit(AstRNGuniform&);
    void visit(AstScalar&);
    void visit(AstTranspose&);
    void visit(AstVariable&);

    void visit(XStmtBarrier&);
    void visit(XStmtCompound&);
    void visit(XStmtCreateData&);
    void visit(XStmtExtension&);
    void visit(XStmtExtensionAuto&);
    void visit(XStmtIdSpace&);
    void visit(XStmtIndex&);
    void visit(XStmtLiteral&);
    void visit(XStmtMatmul&);
    void visit(XStmtMatmulAuto&);
    void visit(XStmtReadData&);
    void visit(XStmtReduce&);
    void visit(XStmtRepeat&);
    void visit(XStmtRNGrand&);
    void visit(XStmtRNGseed&);
    void visit(XStmtSendData&);
    void visit(XStmtSingle&);
};

}; // namespace chai_internal

#endif
