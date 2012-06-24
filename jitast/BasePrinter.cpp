// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
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
#include "BasePrinter.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// print converted trace

ostream& BasePrinter::indent(void)
{
    for (size_t i = 0; i < _indent; i++)
        _os << "  ";

    return _os;
}

void BasePrinter::descendAst(const size_t idx, BaseAst& v)
{
    v.getArg(idx)->accept( *this );
}

void BasePrinter::printFrontMem(const vector< FrontMem* >& vecFrontMem)
{
    for (vector< FrontMem* >::const_iterator
         it = vecFrontMem.begin();
         it != vecFrontMem.end();
         it++)
    {
        _os << "PTR(" << (*it)->ptrMem() << ")";
    }
}

BasePrinter::BasePrinter(ostream& os)
    : _os(os),
      _indent(0) { }

BasePrinter::~BasePrinter(void) { }

////////////////////////////////////////
// VisitAst

void BasePrinter::visit(AstAccum& v)
{
    _os << (v.takeAvg() ? "mean(" : "sum(");
    descendAst(0, v);
    _os << ")";
}

void BasePrinter::visit(AstArrayMem& v)
{
    printFrontMem(v.frontMem());

    _os << " FROM " << v.variable() << "." << v.version();
}

void BasePrinter::visit(AstCond& v)
{
    _os << "cond(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ", ";
    descendAst(2, v);
    _os << ")";
}

void BasePrinter::visit(AstConvert& v)
{
    if (! v.isNOP())
    {
        _os << "convert_";

        switch (v.precision())
        {
            case (PrecType::UInt32) : _os << "u32"; break;
            case (PrecType::Int32) : _os << "i32"; break;
            case (PrecType::Float) : _os << "f32"; break;
            case (PrecType::Double) : _os << "f64"; break;
        }

        _os << "(";
    }

    descendAst(0, v);

    if (! v.isNOP())
        _os << ")";
}

void BasePrinter::visit(AstDotprod& v)
{
    _os << "dot_product(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BasePrinter::visit(AstExtension& v)
{
    _os << v.extensionName() << "(";
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept( *this );

        if (i != v.numArg() - 1)
            _os << ", ";
    }
    _os << ")";
}

void BasePrinter::visit(AstFun1& v)
{
    _os << v.fun().str() << "(";
    descendAst(0, v);
    _os << ")";
}

void BasePrinter::visit(AstFun2& v)
{
    const string funStr = v.fun().str();

    if (v.fun().infix())
    {
        _os << "(";
        descendAst(0, v);
        _os << " " << funStr << " ";
        descendAst(1, v);
        _os << ")";
    }
    else
    {
        _os << funStr << "(";
        descendAst(0, v);
        _os << ", ";
        descendAst(1, v);
        _os << ")";
    }
}

void BasePrinter::visit(AstFun3& v)
{
    const string funStr = v.fun().str();

    _os << funStr << "(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ", ";
    descendAst(2, v);
    _os << ")";
}

void BasePrinter::visit(AstGather& v)
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

void BasePrinter::visit(AstIdxdata& v)
{
    _os << "indexX_";

    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << "(" << v.index() << ", " << v.W() << ", " << v.H() << ")";
}

void BasePrinter::visit(AstLitdata& v)
{
    switch (v.precision())
    {
        case (PrecType::UInt32) :
            _os << (0 == v.uintValue() ? "zeros" : "ones");
            break;
        case (PrecType::Int32) :
            _os << (0 == v.intValue() ? "zeros" : "ones");
            break;
        case (PrecType::Float) :
            _os << (0 == v.floatValue() ? "zeros" : "ones");
            break;
        case (PrecType::Double) :
            _os << (0 == v.doubleValue() ? "zeros" : "ones");
            break;
    }

    _os << "X_";

    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << "(" << v.W() << ", " << v.H() << ")";
}

void BasePrinter::visit(AstMakedata& v)
{
    _os << "makeX_";

    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << "(" << v.W() << ", " << v.H() << ", ";

    printFrontMem(v.frontMem());

    _os << ")";
}

void BasePrinter::visit(AstMatmulMM& v)
{
    _os << "matmulMM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BasePrinter::visit(AstMatmulMV& v)
{
    _os << "matmulMV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BasePrinter::visit(AstMatmulVM& v)
{
    _os << "matmulVM(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BasePrinter::visit(AstMatmulVV& v)
{
    _os << "matmulVV(";
    descendAst(0, v);
    _os << ", ";
    descendAst(1, v);
    _os << ")";
}

void BasePrinter::visit(AstOpenCL& v)
{
    for (vector< string >::const_iterator
         it = v.programSource().begin();
         it != v.programSource().end();
         it++)
    {
        _os << (*it) << endl;
    }
}

void BasePrinter::visit(AstReadout& v)
{
    _os << "readout(";

    printFrontMem(v.frontMem());

    _os << ", ";
    descendAst(0, v);
    _os << ")";
}

void BasePrinter::visit(AstRNGnormal& v)
{
    _os << "rng_normal_make_";

    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << "(" << v.W() << ")";
}

void BasePrinter::visit(AstRNGuniform& v)
{
    _os << "rng_uniform_make_";

    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << "u32"; break;
        case (PrecType::Int32) : _os << "i32"; break;
        case (PrecType::Float) : _os << "f32"; break;
        case (PrecType::Double) : _os << "f64"; break;
    }

    _os << "("
        << v.W() << ", "
        << v.step() << ", "
        << v.minlimit() << ", "
        << v.maxlimit()
        << ")";
}

void BasePrinter::visit(AstScalar& v)
{
    switch (v.precision())
    {
        case (PrecType::UInt32) : _os << v.uintValue(); break;
        case (PrecType::Int32) : _os << v.intValue(); break;
        case (PrecType::Float) : _os << v.floatValue(); break;
        case (PrecType::Double) : _os << v.doubleValue(); break;
    }
}

void BasePrinter::visit(AstTranspose& v)
{
    _os << "transpose(";
    descendAst(0, v);
    _os << ")";
}

void BasePrinter::visit(AstVariable& v)
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

        // bytes to bits
        const size_t precision = PrecType::sizeOf(v.precision()) * 8;

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
