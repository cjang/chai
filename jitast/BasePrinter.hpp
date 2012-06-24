// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BASE_PRINTER_HPP_
#define _CHAI_BASE_PRINTER_HPP_

#include <ostream>
#include <vector>

#include "BaseAst.hpp"
#include "FrontMem.hpp"
#include "VisitAst.hpp"

namespace chai_internal {

////////////////////////////////////////
// print the AST

class BasePrinter : public VisitAst
{
protected:
    std::ostream& _os;

    // indentation
    size_t        _indent;

    // distinguish LHS from RHS during AST descent
    bool          _descendVar;

    std::ostream& indent(void);

    void descendAst(const size_t idx, BaseAst&);

    void printFrontMem(const std::vector< FrontMem* >&);

    BasePrinter(std::ostream&);

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

public:
    virtual ~BasePrinter(void);
};

}; // namespace chai_internal

#endif
