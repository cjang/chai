// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_GATHER_HPP_
#define _CHAI_AST_GATHER_HPP_

#include <set>
#include <stdint.h>
#include <vector>

#include "BaseAst.hpp"
#include "VisitAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// gather1_floor, gather2_floor

class AstGather : public BaseAst,
                  public VisitAst
{
    const size_t _N;

    bool         _eligible;
    AstVariable* _dataVariable;
    bool         _xHasIndex;
    bool         _yHasIndex;
    size_t       _xOffset;
    size_t       _yOffset;

    size_t _countNode;
    size_t _countFun2Add;
    size_t _countFun2Sub;
    size_t _countIdxdataWidth;  // 0 varies across rows
    size_t _countIdxdataHeight; // 1 varies down columns
    size_t _countScalar;
    size_t _countVariable;
    int    _countFloorValue;

    void resetCount(void);
    bool isOneVariable(void) const;
    bool isStaticSubscript(const size_t index01) const;

    void descendAst(BaseAst&);

public:
    AstGather(BaseAst* bargData,
              BaseAst* bargX);

    AstGather(BaseAst* bargData,
              BaseAst* bargX,
              BaseAst* bargY);

    size_t N(void) const;

    bool eligible(void) const;
    AstVariable* dataVariable(void) const;
    bool xHasIndex(void) const;
    bool yHasIndex(void) const;
    size_t xOffset(void) const;
    size_t yOffset(void) const;

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

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
