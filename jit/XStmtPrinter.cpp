// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "XStmtBarrier.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtExtension.hpp"
#include "XStmtExtensionAuto.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtMatmulAuto.hpp"
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

XStmtPrinter::XStmtPrinter(ostream& os,
                           const bool descendIdSpace)
    : BasePrinter(os),
      _descendIdSpace(descendIdSpace),
      _repeatIndex(0) { }

////////////////////////////////////////
// VisitXStmt

void XStmtPrinter::visit(XStmtBarrier& barStmt)
{
    indent() << "BARRIER" << endl;
}

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

void XStmtPrinter::visit(XStmtExtension& extStmt)
{
    indent() << "EXTENSION " << extStmt.extensionName() << endl;
}

void XStmtPrinter::visit(XStmtExtensionAuto& extStmt)
{
    indent() << "EXTENSIONAUTO " << extStmt.extensionName() << endl;
}

void XStmtPrinter::visit(XStmtIdSpace& idsStmt)
{
    if ( 0 == idsStmt.streamW() && 0 == idsStmt.streamH() )
    {
        indent() << "DATA FOR "
                 << idsStmt.numTraces()
                 << " TRACES"
                 << endl;
    }
    else
    {
        indent() << "INDEX SPACE "
                 << idsStmt.streamW()
                 << " x " 
                 << idsStmt.streamH()
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

void XStmtPrinter::visit(XStmtMatmulAuto& mmaStmt)
{
    indent() << "MMAUTO "
             << mmaStmt.widthC() << "x" << mmaStmt.heightC() << "f"
             << (sizeof(double) == mmaStmt.precisionC() ? 64 : 32)
             << " = matmul"
             << (mmaStmt.isTransposeA() ? "T" : "N")
             << (mmaStmt.isTransposeB() ? "T" : "N")
             << "("
             << mmaStmt.widthA() << "x" << mmaStmt.heightA() << "f"
             << (sizeof(double) == mmaStmt.precisionA() ? 64 : 32)
             << ", "
             << mmaStmt.widthB() << "x" << mmaStmt.heightB() << "f"
             << (sizeof(double) == mmaStmt.precisionB() ? 64 : 32)
             << ")"
             << endl;
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

void XStmtPrinter::visit(AstAccum& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstArrayMem& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstCond& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstConvert& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstDotprod& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstFun1& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstFun2& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstFun3& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstGather& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstIdxdata& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstLitdata& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstMakedata& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstMatmulMM& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstMatmulMV& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstMatmulVM& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstMatmulVV& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstReadout& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstRNGnormal& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstRNGuniform& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstScalar& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstTranspose& v) { BasePrinter::visit(v); }
void XStmtPrinter::visit(AstVariable& v) { BasePrinter::visit(v); }

}; // namespace chai_internal
