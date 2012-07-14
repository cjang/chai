// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MAKE_STMT_HPP_
#define _CHAI_MAKE_STMT_HPP_

#include <map>
#include <set>
#include <stack>
#include <utility>
#include <vector>

#include "Stmt.hpp"
#include "VectorTrace.hpp"
#include "VisitAst.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create converted trace

class MakeStmt : public VisitBCStmt,
                 public VisitAst
{
    VectorTrace& _vt;

    // root lexical scope
    std::vector< Stmt* >& _lexRoot;

    // current lexical scope
    std::stack< std::vector< Stmt* >* > _lexScope;

    // support for nested rolled loops
    std::stack< StmtRepeat* > _repScope;

    // distinguish top LHS from RHS variables
    AstVariable* _lhsRoot;
    AstConvert*  _lhsConvert;

    // mark lexical scope of variables
    bool _constructorLHS;
    bool _destructorLHS;

    // keep track of LHS trace variables
    std::set< uint32_t > _lhsTraceVariables;

    // keep track of objects during descent down RHS (clear each statement)
    std::set< BaseAst* > _trackAst;
    std::set< BaseAst* > _trackExtension; // language extension
    std::set< BaseAst* > _trackIndex;
    enum TrackLiteral { LITERAL_DATA, LITERAL_SCALAR };
    std::map< BaseAst*, TrackLiteral > _trackLiteral;
    enum TrackMatmul { MATMUL_MM, MATMUL_MV, MATMUL_VM };
    std::map< BaseAst*, TrackMatmul > _trackMatmul;
    std::set< BaseAst* > _trackMatmulArgs;
    std::set< BaseAst* > _trackOpenCL;
    std::set< BaseAst* > _trackReadData;
    enum TrackReduce { REDUCE_ACCUM, REDUCE_DOTPROD };
    std::map< BaseAst*, TrackReduce > _trackReduce;
    std::set< BaseAst* > _trackSendData;
    std::set< BaseAst* > _trackTranspose;
    std::set< BaseAst* > _trackVar;

    // keep track of transposed array variables (clear each statement)
    std::vector< AstTranspose* > _transposeStack;

    // keep track of ineligible gathered array variables (clear each statement)
    std::stack< AstGather* > _gatherStack;

    // keep track of array memory splits
    std::map< std::pair< uint32_t, uint32_t >, AstVariable* > _memSplit;

    // Array memory appears in three ways.
    //
    // 1. created with make1/2
    //    - by API design, every make1/2 is assigned to a unique LHS variable
    //
    // 2. created with read_scalar, read1, read2
    //    - by API design, this appears once at the end of a kernel trace
    //    - note: the output variable may earlier have been used for input
    //
    // 3. variable value in RHS expression
    //    - sub-expressions may be split off with new LHS variables
    //    - note: the LHS variable may be assigned more than once
    //
    // The desirable trace invariant is single assignment of array memory
    // to variables. Any variable should never be assigned a vector of
    // array memory more than once.
    //
    // There are no concerns with make1/2. The C++ API design enforces the
    // invariant.
    //
    // The read_scalar, read1 and read2 operations are not assigning data
    // to a variable. They are reading back from a variable.
    //
    // If variable assignments are counted, then statement splits can be
    // introduced to maintain the single assignment invariant.

    // keep track of array memory assignments to a variable
    std::set< uint32_t > _assignedVars;

    // readout variables (by definition, these must be from the trace)
    std::map< uint32_t, size_t > _traceReadoutDim;

    // is there a reduction statement inside a loop?
    bool _reductionInsideLoop;

    // does this statement have a transpose or gather forcing vector length 1?
    bool _scalarVecLength;

    // number of containing non-matmul-argument transposes for an AST object
    size_t getTransposeCount(void) const;

    // special tracking for transposed and gathered variables
    void specialTrackVar(void);

    // add new statements
    void pushLexRoot(Stmt*);
    void pushLexScope(Stmt*);
    void pushLexScope(std::vector< Stmt* >*);
    void popLexScope(void);

    // split statements
    void clearAst(void);
    void insertAst(BaseAst&);
    void insertExtension(AstExtension&);
    void insertIndex(AstIdxdata&);
    void insertLiteral(AstLitdata&);
    void insertLiteral(AstScalar&);
    void insertMatmul(AstMatmulMM&);
    void insertMatmul(AstMatmulMV&);
    void insertMatmul(AstMatmulVM&);
    void insertOpenCL(AstOpenCL&);
    void insertReadData(AstReadout&);
    void insertReduce(AstAccum&);
    void insertReduce(AstDotprod&);
    void insertSendData(AstArrayMem&);
    void insertSendData(AstMakedata&);
    void insertTranspose(AstTranspose&);
    void insertVar(AstVariable&);
    enum SplitContext
    {
        DEFAULT,
        FORCE_EXTENSION,
        FORCE_MATMUL
    };
    void splitAst(BaseAst&,
                  const size_t argIndex,
                  const SplitContext force = DEFAULT);
    void splitAst(BaseAst&,
                  const SplitContext force = DEFAULT);

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    MakeStmt(std::vector< Stmt* >&, VectorTrace&);

    const std::map< uint32_t, size_t >& traceReadoutDim(void) const;

    bool reductionInsideLoop(void) const;

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);

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
};

}; // namespace chai_internal

#endif
