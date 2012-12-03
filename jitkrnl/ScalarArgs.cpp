// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstOpenCL.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"
#include "ScalarArgs.hpp"
#include "StmtBarrier.hpp"
#include "StmtCompound.hpp"
#include "StmtCreateData.hpp"
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
// scalar AST objects to kernel arguments

void ScalarArgs::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
        v.getArg(i)->accept(*this);
}

ScalarArgs::ScalarArgs(void)
    : _scalarAST() { }

void ScalarArgs::edit(StmtIdSpace& xid)
{
    // iterate over and traverse within kernel statements
    for (vector< Stmt* >::const_iterator
         it = xid.stuffInside().begin(); it != xid.stuffInside().end(); it++)
    {
        (*it)->accept(*this);
    }

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "scalar AST objects in kernel:";
    for (size_t i = 0; i < _scalarAST.size(); i++)
    {
        const AstScalar* a = _scalarAST[i];

        ss << " [" <<  i << " (";

        switch (a->prec())
        {
            case (PrecType::UInt32) :
                ss << "uint32_t)" << a->uintValue();
                break;

            case (PrecType::Int32) :
                ss << "int32_t)" << a->intValue();
                break;

            case (PrecType::Float) :
                ss << "float)" << a->floatValue();
                break;

            case (PrecType::Double) :
                ss << "double)" << a->doubleValue();
                break;
        }

        ss << "]";
    }

    LOGGER(ss.str())
#endif

    for (vector< AstScalar* >::const_iterator
         it = _scalarAST.begin(); it != _scalarAST.end(); it++)
    {
        xid.setScalarAST(*it);
    }
}

////////////////////////////////////////
// VisitAst

void ScalarArgs::visit(AstAccum& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstArrayMem& v)
{
}

void ScalarArgs::visit(AstCond& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstConvert& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstDotprod& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstFun1& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstFun2& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstFun3& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstGather& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstIdxdata& v)
{
}

void ScalarArgs::visit(AstLitdata& v)
{
}

void ScalarArgs::visit(AstMakedata& v)
{
}

void ScalarArgs::visit(AstMatmulMM& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstMatmulMV& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstMatmulVM& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstMatmulVV& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstOpenCL& v)
{
}

void ScalarArgs::visit(AstReadout& v)
{
}

void ScalarArgs::visit(AstRNGnormal& v)
{
}

void ScalarArgs::visit(AstRNGuniform& v)
{
}

void ScalarArgs::visit(AstScalar& v)
{
    _scalarAST.push_back( &v );
}

void ScalarArgs::visit(AstTranspose& v)
{
    descendAst(v);
}

void ScalarArgs::visit(AstVariable& v)
{
}

////////////////////////////////////////
// VisitStmt

void ScalarArgs::visit(StmtBarrier& s)
{
    // leave this alone, no RHS
}

void ScalarArgs::visit(StmtCompound& s)
{
    // need to look in contained statements
    for (vector< Stmt* >::const_iterator
         it = s.stuffInside().begin(); it != s.stuffInside().end(); it++)
    {
        (*it)->accept(*this);
    }
}

void ScalarArgs::visit(StmtCreateData& s)
{
    // leave this alone, no RHS
}

void ScalarArgs::visit(StmtGatherAuto& s)
{
    // leave this alone, meta-kernel
}

void ScalarArgs::visit(StmtIdSpace& s)
{
    // should never happen
}

void ScalarArgs::visit(StmtIndex& s)
{
    // leave this alone, no scalar AST objects on RHS
}

void ScalarArgs::visit(StmtLiteral& s)
{
    // look for scalar AST objects on RHS
    if (s.scalarPtr()) s.scalarPtr()->accept(*this);
}

void ScalarArgs::visit(StmtMatmul& s)
{
    // leave this alone, is separate kernel
}

void ScalarArgs::visit(StmtMatmulAuto& s)
{
    // leave this alone, meta-kernel
}

void ScalarArgs::visit(StmtOpenCL& s)
{
    // leave this alone, is separate kernel
}

void ScalarArgs::visit(StmtReadData& s)
{
    // leave this alone, meta-kernel
}

void ScalarArgs::visit(StmtReduce& s)
{
    // look for scalar AST objects on RHS
    if (s.accumPtr()) s.accumPtr()->accept(*this);
    if (s.dotprodPtr()) s.dotprodPtr()->accept(*this);
}

void ScalarArgs::visit(StmtRepeat& s)
{
    // need to look in contained statements
    s.stuffInside()->accept(*this);
}

void ScalarArgs::visit(StmtSendData& s)
{
    // leave this alone, meta-kernel
}

void ScalarArgs::visit(StmtSingle& s)
{
    // look for scalar AST objects on RHS
    s.lhsVariable()->getArg(0)->accept(*this);
}

}; // namespace chai_internal
