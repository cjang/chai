// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <vector>

#include "AstOpenCL.hpp"
#include "PrecType.hpp"
#include "PrintTrace.hpp"
#include "StmtBarrier.hpp"
#include "StmtCompound.hpp"
#include "StmtCreateData.hpp"
#include "StmtExtension.hpp"
#include "StmtExtensionAuto.hpp"
#include "StmtGatherAuto.hpp"
#include "StmtIdSpace.hpp"
#include "StmtIndex.hpp"
#include "StmtLiteral.hpp"
#include "StmtMatmul.hpp"
#include "StmtMatmulAuto.hpp"
#include "StmtOpenCL.hpp"
#include "StmtReadData.hpp"
#include "StmtReduce.hpp"
#include "StmtRepeat.hpp"
#include "StmtSendData.hpp"
#include "StmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print converted trace

PrintTrace::PrintTrace(ostream& os,
                       const bool descendIdSpace)
    : BasePrinter(os),
      _descendIdSpace(descendIdSpace),
      _repeatIndex(0) { }

////////////////////////////////////////
// VisitStmt

void PrintTrace::visit(StmtBarrier& barStmt)
{
    indent() << "BARRIER" << endl;
}

void PrintTrace::visit(StmtCompound& comStmt)
{
    // visit each statement inside compound
    for (vector< Stmt* >::const_iterator
         it = comStmt.stuffInside().begin();
         it != comStmt.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void PrintTrace::visit(StmtCreateData& creStmt)
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

void PrintTrace::visit(StmtExtension& extStmt)
{
    indent() << "EXTENSION " << extStmt.extensionName() << endl;
}

void PrintTrace::visit(StmtExtensionAuto& extStmt)
{
    indent() << "EXTENSIONAUTO " << extStmt.extensionName() << endl;
}

void PrintTrace::visit(StmtGatherAuto& gatStmt)
{
    indent() << "GATHERAUTO ";

    visit(*gatStmt.dataPtr());

    _os << endl;
}

void PrintTrace::visit(StmtIdSpace& idsStmt)
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
        for (vector< Stmt* >::const_iterator
             it = idsStmt.stuffInside().begin();
             it != idsStmt.stuffInside().end();
             it++)
        {
            (*it)->accept(*this);
        }
    }
}

void PrintTrace::visit(StmtIndex& idxStmt)
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

void PrintTrace::visit(StmtLiteral& litStmt)
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

void PrintTrace::visit(StmtMatmul& matStmt)
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

void PrintTrace::visit(StmtMatmulAuto& mmaStmt)
{
    indent() << "MMAUTO " << mmaStmt.widthC() << "x" << mmaStmt.heightC();

    switch (mmaStmt.precC())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << " = matmul"
        << (mmaStmt.isTransposeA() ? "T" : "N")
        << (mmaStmt.isTransposeB() ? "T" : "N")
        << "("
        << mmaStmt.widthA() << "x" << mmaStmt.heightA();

    switch (mmaStmt.precA())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << ", " << mmaStmt.widthB() << "x" << mmaStmt.heightB();

    switch (mmaStmt.precB())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << ")" << endl;
}

void PrintTrace::visit(StmtOpenCL& oclStmt)
{
    indent() << "OPENCL "
             << oclStmt.openclPtr()->programHashCode() << " "
             << oclStmt.openclPtr()->kernelName()
             << endl;
}

void PrintTrace::visit(StmtReadData& datStmt)
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

void PrintTrace::visit(StmtReduce& redStmt)
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

void PrintTrace::visit(StmtRepeat& repStmt)
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

void PrintTrace::visit(StmtSendData& datStmt)
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

void PrintTrace::visit(StmtSingle& sinStmt)
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

void PrintTrace::visit(AstAccum& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstArrayMem& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstCond& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstConvert& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstDotprod& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstFun1& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstFun2& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstFun3& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstGather& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstIdxdata& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstLitdata& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstMakedata& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstMatmulMM& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstMatmulMV& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstMatmulVM& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstMatmulVV& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstOpenCL& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstReadout& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstRNGnormal& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstRNGuniform& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstScalar& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstTranspose& v) { BasePrinter::visit(v); }
void PrintTrace::visit(AstVariable& v) { BasePrinter::visit(v); }

}; // namespace chai_internal
