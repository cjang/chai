// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string>

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstBinop.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstIsomorph.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstVariable.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtPrinter.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print bytecode statement trace

ostream& BCStmtPrinter::indent(void)
{
    for (size_t i = 0; i < _indent; i++)
        _os << "  ";

    return _os;
}

void BCStmtPrinter::descendAst(const size_t idx, BaseAst& obj)
{
    obj.getArg(idx)->accept(*this);
}

BCStmtPrinter::BCStmtPrinter(ostream& os)
    : _os(os),
      _indent(0),
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

void BCStmtPrinter::visit(AstAccum& v)
{
    _os << (v.takeAvg() ? "mean(" : "sum(");
    descendAst(0, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstArrayMem& v)
{
    for (vector< FrontMem* >::const_iterator
         it = v.frontMem().begin();
         it != v.frontMem().end();
         it++)
    {
        _os << "PTR(" << (*it)->ptrMem() << ")";
    }
}

void BCStmtPrinter::visit(AstBinop& v)
{
    const string funStr = v.fun().str();

    if ("fmax" == funStr || "fmin" == funStr)
    {
        _os << funStr << "(";
        descendAst(0, v);
        _os << ", ";
        descendAst(1, v);
        _os << ")";
    }
    else
    {
        _os << "(";
        descendAst(0, v);
        _os << " " << funStr << " ";
        descendAst(1, v);
        _os << ")";
    }
}

void BCStmtPrinter::visit(AstCond& v)
{
    _os << "cond(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ", ";
    descendAst(2, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstConvert& v)
{
    if (! v.isNOP())
        _os << "convert_" << (v.isDouble() ? "f64" : "f32") << "(";

    descendAst(0, v);

    if (! v.isNOP())
        _os << ")";
}

void BCStmtPrinter::visit(AstDotprod& v)
{
    _os << "dot_product(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstGather& v)
{
    _os << "gather" << v.N() << "_floor(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    if (2 == v.N())
    {
        _os << ", ";
        descendAst(2, v);
    }
    _os << ")";
}

void BCStmtPrinter::visit(AstIdxdata& v)
{
    _os << "indexX_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.index() << ", " << v.W() << ", " << v.H() << ")";
}

void BCStmtPrinter::visit(AstIsomorph& v)
{
    _os << v.fun().str() << "(";
    descendAst(0, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstLitdata& v)
{
    if (v.isDouble())
        _os << (0 == v.doubleValue() ? "zeros" : "ones");
    else
        _os << (0 == v.floatValue() ? "zeros" : "ones");

    _os << "X_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W() << ", " << v.H() << ")";
}

void BCStmtPrinter::visit(AstMakedata& v)
{
    _os << "makeX_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W() << ", " << v.H() << ", ";

    for (vector< FrontMem* >::const_iterator
         it = v.frontMem().begin();
         it != v.frontMem().end();
         it++)
    {
        _os << "PTR(" << (*it)->ptrMem() << ")";
    }

    _os << ")";
}

void BCStmtPrinter::visit(AstMatmulMM& v)
{
    _os << "matmulMM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstMatmulMV& v)
{
    _os << "matmulMV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstMatmulVM& v)
{
    _os << "matmulVM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstMatmulVV& v)
{
    _os << "matmulVV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BCStmtPrinter::visit(AstReadout& v)
{
    _os << "readout(";

    for (vector< FrontMem* >::const_iterator
         it = v.frontMem().begin();
         it != v.frontMem().end();
         it++)
    {
        _os << "PTR(" << (*it)->ptrMem() << ")";
    }

    _os << ", ";

    descendAst(0, v);

    _os << ")";
}

void BCStmtPrinter::visit(AstRNGnormal& v)
{
    _os << "rng_normal_make_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W()
        << ")";
}

void BCStmtPrinter::visit(AstRNGuniform& v)
{
    _os << "rng_uniform_make_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W() << ", "
        << v.step() << ", "
        << v.minlimit() << ", "
        << v.maxlimit()
        << ")";
}

void BCStmtPrinter::visit(AstScalar& v)
{
    if (v.isDouble())
        _os << v.doubleValue();
    else
        _os << v.floatValue();
}

void BCStmtPrinter::visit(AstVariable& v)
{
    if (_descendVar)
    {
        _descendVar = false;
        descendAst(0, v);
    }
    else
    {
        if (v.isTraceVariable())
            _os << "VAR(" << v.variable() << ")";
        else
            _os << "VAR(" << &v << ")";
    }
}

}; // namespace chai_internal
