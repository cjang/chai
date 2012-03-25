// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_EXTENSION_HPP_
#define _CHAI_STMT_EXTENSION_HPP_

#include "AstExtension.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"
#include "VisitAst.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// auto-tuned language extension kernel

class StmtExtension : public Stmt,
                      public VisitAst
{
    AstExtension* _extObj;

    void descendAst(BaseAst&);

public:
    StmtExtension(AstVariable*, AstExtension*);
    ~StmtExtension(void);

    std::string extensionName(void) const;
    bool extensionEnqueue(MemManager&, VectorTrace&);

    virtual bool swappable(const Stmt&) const;
    void accept(VisitStmt&);

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
};

}; // namespace chai_internal

#endif
