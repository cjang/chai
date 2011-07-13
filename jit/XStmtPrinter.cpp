// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

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
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtGather.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtPrinter.hpp"
#include "XStmtReadData.hpp"
#include "XStmtReduce.hpp"
#include "XStmtRepeat.hpp"
#include "XStmtRNGrand.hpp"
#include "XStmtRNGseed.hpp"
#include "XStmtSendData.hpp"
#include "XStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print converted trace

ostream& XStmtPrinter::indent(void)
{
    for (size_t i = 0; i < _indent; i++)
        _os << "  ";

    return _os;
}

void XStmtPrinter::descendAst(const size_t idx, BaseAst& v)
{
    v.getArg(idx)->accept( *this );
}

XStmtPrinter::XStmtPrinter(ostream& os,
                           const bool descendIdSpace)
    : _descendIdSpace(descendIdSpace),
      _os(os),
      _indent(0),
      _repeatIndex(0) { }

////////////////////////////////////////
// VisitXStmt

void XStmtPrinter::visit(XStmtCompound& comStmt)
{
    // visit each statement inside compound
    for (vector< XStmt* >::const_iterator
         it = comStmt.stuffInside().begin();
         it != comStmt.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void XStmtPrinter::visit(XStmtCreateData& creStmt)
{
    indent() << "CREATE ";

    _os << creStmt.lhsVariable()->W()
        << "x"
        << creStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*creStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtGather& gatStmt)
{
    indent() << "GATHER ";

    visit(*gatStmt.gatherPtr());

    _os << " TO ";

    _os << gatStmt.lhsVariable()->W()
        << "x"
        << gatStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*gatStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtIdSpace& idsStmt)
{
    if ( 0 == idsStmt.indexW() && 0 == idsStmt.indexH() )
    {
        indent() << "DATA FOR "
                 << idsStmt.numTraces()
                 << " TRACES"
                 << endl;
    }
    else
    {
        indent() << "INDEX SPACE "
                 << idsStmt.indexW()
                 << " x " 
                 << idsStmt.indexH()
                 << " FOR "
                 << idsStmt.numTraces()
                 << " TRACES"
                 << endl;
    }

    if (_descendIdSpace)
    {
        // visit each statement inside
        for (vector< XStmt* >::const_iterator
             it = idsStmt.stuffInside().begin();
             it != idsStmt.stuffInside().end();
             it++)
        {
            (*it)->accept(*this);
        }
    }
}

void XStmtPrinter::visit(XStmtIndex& idxStmt)
{
    indent() << "INDEX ";

    visit(*idxStmt.idxdataPtr());

    _os << " TO ";

    _os << idxStmt.lhsVariable()->W()
        << "x"
        << idxStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*idxStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtLiteral& litStmt)
{
    indent() << "LITERAL ";

    // only one of these will be true
    if (litStmt.litdataPtr()) visit(*litStmt.litdataPtr());
    if (litStmt.scalarPtr()) visit(*litStmt.scalarPtr());

    _os << " TO ";

    _os << litStmt.lhsVariable()->W()
        << "x"
        << litStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*litStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtMatmul& matStmt)
{
    indent() << "MATMUL ";

    // only one of these will be true
    if (matStmt.matmulPtr()) visit(*matStmt.matmulPtr());
    if (matStmt.matvecPtr()) visit(*matStmt.matvecPtr());
    if (matStmt.vecmatPtr()) visit(*matStmt.vecmatPtr());

    _os << " TO ";

    _os << matStmt.lhsVariable()->W()
        << "x"
        << matStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*matStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtReadData& datStmt)
{
    indent() << "READ ";

    visit(*datStmt.readoutPtr());

    _os << " TO ";

    _os << datStmt.lhsVariable()->W()
        << "x"
        << datStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*datStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtReduce& redStmt)
{
    indent() << "REDUCE ";

    // only one of these will be true
    if (redStmt.accumPtr()) visit(*redStmt.accumPtr());
    if (redStmt.dotprodPtr()) visit(*redStmt.dotprodPtr());

    _os << " TO ";

    _os << redStmt.lhsVariable()->W()
        << "x"
        << redStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*redStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtRepeat& repStmt)
{
    if (repStmt.getLoopRoll())
    {
        // begin loop
        indent()
            << "for (int i" << _repeatIndex << " = 0; "
                        "i" << _repeatIndex << " < "
                            << repStmt.numReps() << "; "
                        "i" << _repeatIndex << "++)" << endl;
        indent()
            << "{" << endl;

        _repeatIndex++;
        _indent++;

        // visit statement 
        repStmt.stuffInside()->accept(*this);

        _indent--;

        // end loop
        indent()
            << "}" << endl;
    }
    else
    {
        for (size_t i = 0; i < repStmt.numReps(); i++)
        {
            // visit statement 
            repStmt.stuffInside()->accept(*this);
        }
    }
}

void XStmtPrinter::visit(XStmtRNGrand& rngStmt)
{
    indent() << "RNG ";

    // only one of these will be true
    if (rngStmt.normalPtr()) visit(*rngStmt.normalPtr());
    if (rngStmt.uniformPtr()) visit(*rngStmt.uniformPtr());

    _os << " TO ";

    _os << rngStmt.lhsVariable()->W()
        << "x"
        << rngStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*rngStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtRNGseed& rngStmt)
{
    indent() << "RNGSEED " << rngStmt.length() << endl;
}

void XStmtPrinter::visit(XStmtSendData& datStmt)
{
    indent() << "SEND ";

    // only one of these will be true
    if (datStmt.arrayMemPtr()) visit(*datStmt.arrayMemPtr());
    if (datStmt.makedataPtr()) visit(*datStmt.makedataPtr());

    _os << " TO ";

    _os << datStmt.lhsVariable()->W()
        << "x"
        << datStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    visit(*datStmt.lhsVariable());

    _os << endl;
}

void XStmtPrinter::visit(XStmtSingle& sinStmt)
{
    indent();

    _os << sinStmt.lhsVariable()->W()
        << "x"
        << sinStmt.lhsVariable()->H()
        << " ";

    _descendVar = false;
    sinStmt.lhsVariable()->accept(*this);

    _os << " <- ";

    _descendVar = true;
    sinStmt.lhsVariable()->accept(*this);

    _os << endl;
}

////////////////////////////////////////
// VisitAst

void XStmtPrinter::visit(AstAccum& v)
{
    _os << (v.takeAvg() ? "mean(" : "sum(");
    descendAst(0, v);
    _os << ")";
}

void XStmtPrinter::visit(AstArrayMem& v)
{
    for (vector< FrontMem* >::const_iterator
         it = v.frontMem().begin();
         it != v.frontMem().end();
         it++)
    {
        _os << "PTR(" << (*it)->ptrMem() << ")";
    }

    _os << " FROM " << v.variable() << "." << v.version();
}

void XStmtPrinter::visit(AstBinop& v)
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

void XStmtPrinter::visit(AstCond& v)
{
    _os << "cond(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ", ";
    descendAst(2, v);
    _os << ")";
}

void XStmtPrinter::visit(AstConvert& v)
{
    if (! v.isNOP()) _os << "convert_" << (v.isDouble() ? "f64" : "f32") << "(";
    descendAst(0, v);
    if (! v.isNOP()) _os << ")";
}

void XStmtPrinter::visit(AstDotprod& v)
{
    _os << "dot_product(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void XStmtPrinter::visit(AstGather& v)
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

void XStmtPrinter::visit(AstIdxdata& v)
{
    _os << "indexX_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.index() << ", " << v.W() << ", " << v.H() << ")";
}

void XStmtPrinter::visit(AstIsomorph& v)
{
    _os << v.fun().str() << "(";
    descendAst(0, v);
    _os << ")";
}

void XStmtPrinter::visit(AstLitdata& v)
{
    if (v.isDouble())
        _os << (0 == v.doubleValue() ? "zeros" : "ones");
    else
        _os << (0 == v.floatValue() ? "zeros" : "ones");

    _os << "X_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W() << ", " << v.H() << ")";
}

void XStmtPrinter::visit(AstMakedata& v)
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

void XStmtPrinter::visit(AstMatmulMM& v)
{
    _os << "matmulMM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void XStmtPrinter::visit(AstMatmulMV& v)
{
    _os << "matmulMV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void XStmtPrinter::visit(AstMatmulVM& v)
{
    _os << "matmulVM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void XStmtPrinter::visit(AstMatmulVV& v)
{
    _os << "matmulVV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void XStmtPrinter::visit(AstReadout& v)
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

void XStmtPrinter::visit(AstRNGnormal& v)
{
    _os << "rng_normal_make_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W()
        << ")";
}

void XStmtPrinter::visit(AstRNGuniform& v)
{
    _os << "rng_uniform_make_" << (v.isDouble() ? "f64" : "f32") << "("
        << v.W() << ", "
        << v.step() << ", "
        << v.minlimit() << ", "
        << v.maxlimit()
        << ")";
}

void XStmtPrinter::visit(AstScalar& v)
{
    if (v.isDouble())
        _os << v.doubleValue();
    else
        _os << v.floatValue();
}

void XStmtPrinter::visit(AstVariable& v)
{
    if (_descendVar)
    {
        // RHS

        _descendVar = false;
        descendAst(0, v);
    }
    else
    {
        // LHS

        const string prefix = v.isLiveVariable() ? "VAR" : "var";

        const size_t precision = v.precision() * 8; // bytes to bits

        _os << prefix << precision << "(";

        if (v.isTraceVariable())
        {
            _os << v.variable() << "." << v.version();
        }
        else
        {
            _os << &v;
        }

        _os << ")";
    }
}

}; // namespace chai_internal
