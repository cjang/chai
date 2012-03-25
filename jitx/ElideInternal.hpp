// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ELIDE_INTERNAL_HPP_
#define _CHAI_ELIDE_INTERNAL_HPP_

#include <map>
#include <set>
#include <stdint.h>

#include "Stmt.hpp"
#include "VisitAst.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// variables lexically scoped within
// kernel are internal and elided

class ElideInternal : public VisitAst,
                      public VisitStmt
{
    const std::set< uint32_t > _candidates;

    // statement -> LHS variable(s)
    // (barriers have multiple LHS variables from preceeding statements)
    std::map< Stmt*, std::set< uint32_t > > _markRemove;

    // these variables are not elided
    std::set< uint32_t >                    _markKeep;

    // elided trace variables
    std::map< BaseAst*, uint32_t > _trackVar;
    std::map< uint32_t, BaseAst* > _editVars;

    void replaceAst(BaseAst&, const size_t);
    void replaceAst(BaseAst&);

    void descendAst(BaseAst&);

    void keepVariable(const AstVariable*);

public:
    ElideInternal(const std::set< uint32_t>& candidates);

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
