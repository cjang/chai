// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VISIT_AST_HPP_
#define _CHAI_VISIT_AST_HPP_

namespace chai_internal {

class AstAccum;
class AstArrayMem;
class AstBinop;
class AstCond;
class AstConvert;
class AstDotprod;
class AstGather;
class AstIdxdata;
class AstIsomorph;
class AstLitdata;
class AstMakedata;
class AstMatmulMM;
class AstMatmulMV;
class AstMatmulVM;
class AstMatmulVV;
class AstReadout;
class AstRNGnormal;
class AstRNGuniform;
class AstScalar;
class AstVariable;

////////////////////////////////////////
// visitor for JIT syntax trees

struct VisitAst
{
    // specific to JIT
    virtual void visit(AstAccum&) = 0;
    virtual void visit(AstArrayMem&) = 0;
    virtual void visit(AstBinop&) = 0;
    virtual void visit(AstCond&) = 0;
    virtual void visit(AstConvert&) = 0;
    virtual void visit(AstDotprod&) = 0;
    virtual void visit(AstGather&) = 0;
    virtual void visit(AstIdxdata&) = 0;
    virtual void visit(AstIsomorph&) = 0;
    virtual void visit(AstLitdata&) = 0;
    virtual void visit(AstMakedata&) = 0;
    virtual void visit(AstMatmulMM&) = 0;
    virtual void visit(AstMatmulMV&) = 0;
    virtual void visit(AstMatmulVM&) = 0;
    virtual void visit(AstMatmulVV&) = 0;
    virtual void visit(AstReadout&) = 0;
    virtual void visit(AstRNGnormal&) = 0;
    virtual void visit(AstRNGuniform&) = 0;
    virtual void visit(AstScalar&) = 0;
    virtual void visit(AstVariable&) = 0;
};

}; // namespace chai_internal

#endif
