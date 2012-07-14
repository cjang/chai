// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "BCStmtCompound.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"
#include "PrintBCStmt.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print bytecode statement trace

PrintBCStmt::PrintBCStmt(ostream& os)
    : BasePrinter(os),
      _repeatIndex(0) { }

////////////////////////////////////////
// VisitBCStmt

void PrintBCStmt::visit(BCStmtCompound& s)
{
    // visit each statement inside compound
    for (vector< BCStmt* >::const_iterator
         it = s.stuffInside().begin(); it != s.stuffInside().end(); it++)
    {
        (*it)->accept(*this);
    }
}

void PrintBCStmt::visit(BCStmtRepeat& s)
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

    _repeatIndex--;
    _indent--;

    // end loop
    indent() << "}" << endl;
}

void PrintBCStmt::visit(BCStmtSingle& s)
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

void PrintBCStmt::visit(AstAccum& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstArrayMem& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstCond& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstConvert& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstDotprod& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstFun1& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstFun2& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstFun3& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstGather& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstIdxdata& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstLitdata& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstMakedata& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstMatmulMM& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstMatmulMV& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstMatmulVM& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstMatmulVV& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstOpenCL& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstReadout& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstRNGnormal& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstRNGuniform& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstScalar& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstTranspose& v) { BasePrinter::visit(v); }
void PrintBCStmt::visit(AstVariable& v) { BasePrinter::visit(v); }

}; // namespace chai_internal
