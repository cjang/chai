// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_MAKER_HPP_
#define _CHAI_X_STMT_MAKER_HPP_

#include <map>
#include <set>
#include <stack>
#include <utility>
#include <vector>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "XStmt.hpp"
#include "VisitAst.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create converted trace

class XStmtMaker : public VisitBCStmt,
                   public VisitAst
{
    // root lexical scope
    std::vector< XStmt* >& _lexRoot;

    // current lexical scope
    std::stack< std::vector< XStmt* >* > _lexScope;

    // support for nested rolled loops
    std::stack< XStmtRepeat* >           _repScope;

    // distinguish top LHS from RHS variables
    AstVariable* _lhsRoot;
    AstConvert*  _lhsConvert;

    // keep track of LHS trace variables
    std::set< uint32_t > _lhsTraceVariables;

    // keep track of RNG seeds
    std::set< size_t > _rngSeeds;

    // keep track of objects during descent down RHS
    std::set< BaseAst* > _trackAst;
    std::set< BaseAst* > _trackGather;
    std::set< BaseAst* > _trackIndex;
    enum TrackLiteral { LITERAL_DATA, LITERAL_SCALAR };
    std::map< BaseAst*, TrackLiteral > _trackLiteral;
    enum TrackMatmul { MATMUL_MM, MATMUL_MV, MATMUL_VM };
    std::map< BaseAst*, TrackMatmul > _trackMatmul;
    std::set< BaseAst* > _trackReadData;
    enum TrackReduce { REDUCE_ACCUM, REDUCE_DOTPROD };
    std::map< BaseAst*, TrackReduce > _trackReduce;
    enum TrackRNG { RNG_NORMAL, RNG_UNIFORM };
    std::map< BaseAst*, TrackRNG > _trackRNG;
    std::set< BaseAst* > _trackSendData;
    std::set< BaseAst* > _trackVar;

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

    // add new statements
    void pushLexRoot(XStmt*);
    void pushLexScope(XStmt*);
    void pushLexScope(std::vector< XStmt* >*);
    void popLexScope(void);

    // split statements
    void clearAst(void);
    void insertAst(BaseAst&);
    void insertGather(BaseAst&);
    void insertIndex(BaseAst&);
    void insertLiteral(AstLitdata&);
    void insertLiteral(AstScalar&);
    void insertMatmul(AstMatmulMM&);
    void insertMatmul(AstMatmulMV&);
    void insertMatmul(AstMatmulVM&);
    void insertReadData(AstReadout&);
    void insertReduce(AstAccum&);
    void insertReduce(AstDotprod&);
    void insertRNG(AstRNGnormal&);
    void insertRNG(AstRNGuniform&);
    void insertSendData(AstArrayMem&);
    void insertSendData(AstMakedata&);
    void insertVar(AstVariable&);
    enum SplitContext
    {
        DEFAULT,
        FORCE_MATMUL
    };
    void splitAst(BaseAst&, const SplitContext force = DEFAULT);

    // recursive visiting down AST tree
    void descendAst(BaseAst&);

public:
    XStmtMaker(std::vector< XStmt* >&);

    void visit(BCStmtCompound&);
    void visit(BCStmtRepeat&);
    void visit(BCStmtSingle&);

    void visit(AstAccum&);
    void visit(AstArrayMem&);
    void visit(AstBinop&);
    void visit(AstCond&);
    void visit(AstConvert&);
    void visit(AstDotprod&);
    void visit(AstGather&);
    void visit(AstIdxdata&);
    void visit(AstIsomorph&);
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
    void visit(AstVariable&);
};

}; // namespace chai_internal

#endif
