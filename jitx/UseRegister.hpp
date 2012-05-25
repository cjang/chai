// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_USE_REGISTER_HPP_
#define _CHAI_USE_REGISTER_HPP_

#include <map>
#include <set>
#include <stack>
#include <stdint.h>

#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "VisitAst.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// use private register variables when
// possible instead of global memory

class UseRegister : public VisitAst,
                    public VisitStmt
{
    // count of variable reads and writes
    std::map< uint32_t, size_t >           _traceRead;
    std::map< uint32_t, size_t >           _traceWrite;
    std::map< uint32_t, size_t >           _traceReadWrite;

    // first statement must be assignment with LHS only (initialize accum)
    std::map< uint32_t, bool > _traceFirstIsAssign;

    // variables inside reductions or gathers must be memory access
    std::set< uint32_t > _traceInclude;
    std::set< uint32_t > _traceExclude;

/*FIXME - remove this
    // RNG
    std::set< uint32_t > _rngVariables;
*/

/*FIXME - remove this
    std::set< uint32_t > _forceWriteback;
*/

    // keep track of reductions and gathers when descending AST
    std::stack< const BaseAst* > _trackContained;

    // keep track of loops for candidate private registers
    std::stack< const StmtRepeat* > _trackRepeat;

    // memory barriers that may be removed
    std::set< const StmtBarrier* > _trackBarriers;

    // track candidate variables for current statement
    std::set< const AstVariable* > _stmtRHS;

    void updateRW(const AstVariable* stmtLHS);

    void descendAst(BaseAst&);

public:
    UseRegister(void);

    void edit(StmtIdSpace&);

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
    void visit(StmtReadData&);
    void visit(StmtReduce&);
    void visit(StmtRepeat&);
    void visit(StmtSendData&);
    void visit(StmtSingle&);
};

}; // namespace chai_internal

#endif
