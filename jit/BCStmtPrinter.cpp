// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "BCStmtCompound.hpp"
#include "BCStmtPrinter.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print bytecode statement trace

BCStmtPrinter::BCStmtPrinter(ostream& os)
    : BasePrinter(os),
      _repeatIndex(0) { }

////////////////////////////////////////
// VisitBCStmt

void BCStmtPrinter::visit(BCStmtCompound& s)
{
    // visit each statement inside compound
    for (vector< BCStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void BCStmtPrinter::visit(BCStmtRepeat& s)
{
    // begin loop
    indent() << "for (int i" << _repeatIndex << " = 0; "
                         "i" << _repeatIndex << " < " << s.numReps() << "; "
                         "i" << _repeatIndex << "++)" << endl;
    indent() << "{" << endl;

    _repeatIndex++;
    _indent++;

    // visit statement 
    s.stuffInside()->accept(*this);

    _indent--;

    // end loop
    indent() << "}" << endl;
}

void BCStmtPrinter::visit(BCStmtSingle& s)
{
    if (NULL == s.getAst())
        return;

    indent();

    // LHS
    _descendVar = false;
    s.getAst()->accept(*this);

    _os << " <- ";

    // RHS
    _descendVar = true;
    s.getAst()->accept(*this);

    _os << endl;
}

////////////////////////////////////////
// VisitAst

void BCStmtPrinter::visit(AstAccum& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstArrayMem& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstCond& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstConvert& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstDotprod& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstFun1& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstFun2& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstFun3& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstGather& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstIdxdata& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstLitdata& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstMakedata& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstMatmulMM& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstMatmulMV& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstMatmulVM& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstMatmulVV& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstReadout& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstRNGnormal& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstRNGuniform& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstScalar& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstTranspose& v) { BasePrinter::visit(v); }
void BCStmtPrinter::visit(AstVariable& v) { BasePrinter::visit(v); }

}; // namespace chai_internal
