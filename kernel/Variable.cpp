// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Function.hpp"
#include "PrecType.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// abstract base class for variables

Variable::Variable(const size_t PREC,
                   const size_t vecLen)
    : _prec(PREC),
      _vecLen(vecLen),
      _func(NULL) { }

Variable::~Variable(void) { }

void Variable::setFunction(Function* funcPtr)
{
    _func = funcPtr;
}

void Variable::identifierName(ostream& os) const
{
    os << _func->varNameLookup(this);
}

size_t Variable::prec(void) const
{
    return _prec;
}

size_t Variable::vecLength(void) const
{
    return _vecLen;
}

////////////////////////////////////////
// image sampler

Sampler::Sampler(void)
    : Variable(-1, -1) { }

void Sampler::declareType(ostream& os) const
{
    os << "const sampler_t ";
}

void Sampler::convertType(ostream& os) const
{
    // don't do anything for texture samplers
}

////////////////////////////////////////
// image texture

Image2D::Image2D(const size_t PREC,
                 const size_t vecLen,
                 const ImageAccess& rw)
    : Variable(PREC, vecLen),
      _imgAccess(rw) { }

void Image2D::declareType(ostream& os) const
{
    os << _imgAccess.str() << "image2d_t ";
}

void Image2D::convertType(ostream& os) const
{
    // don't do anything for texture samplers
}

////////////////////////////////////////
// variable declaration

ConstVariableDecl::ConstVariableDecl(void) { }
PointerVariableDecl::PointerVariableDecl(void) { }
ConstPointerVariableDecl::ConstPointerVariableDecl(void) { }

////////////////////////////////////////
// memory buffer

AddrMem::AddrMem(const size_t PREC,
                 const size_t vecLen,
                 const AddressSpace& qualifier)
    : Variable(PREC, vecLen),
      _isConst(false),
      _isPointer(false),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t PREC,
                 const size_t vecLen,
                 const ConstVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : Variable(PREC, vecLen),
      _isConst(true),
      _isPointer(false),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t PREC,
                 const size_t vecLen,
                 const PointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : Variable(PREC, vecLen),
      _isConst(false),
      _isPointer(true),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t PREC,
                 const size_t vecLen,
                 const ConstPointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : Variable(PREC, vecLen),
      _isConst(true),
      _isPointer(true),
      _addrSpace(qualifier) { }

bool AddrMem::fp64(void) const
{
    return PrecType::Double == prec();
}

void AddrMem::declareType(ostream& os) const
{
    os << _addrSpace.str();

    if (_isConst) os << "const ";

    os << PrecType::getName(prec());

    if (vecLength() > 1) os << vecLength();

    os << (_isPointer ? " * " : " ");
}

void AddrMem::convertType(ostream& os) const
{
    os << "(";

    os << PrecType::getName(prec());

    if (vecLength() > 1) os << vecLength();

    os << ")";
}

////////////////////////////////////////
// integer scalar

Integer::Integer(const AddressSpace& qualifier)
    : Variable(PrecType::Int32, 1),
      _isConst(false),
      _addrSpace(qualifier) { }

Integer::Integer(const ConstVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : Variable(PrecType::Int32, 1),
      _isConst(true),
      _addrSpace(qualifier) { }

void Integer::declareType(ostream& os) const
{
    os << _addrSpace.str();

    if (_isConst) os << "const ";

    os << "int ";
}

void Integer::convertType(ostream& os) const
{
    // this is a scalar so no type conversion is required
}

////////////////////////////////////////
// (source kernel) array variable

ArrayVariable::ArrayVariable(const size_t PREC,
                             const uint32_t varNum)
    : Variable(PREC, 0),
      _variable(varNum) { }

uint32_t ArrayVariable::variable(void) const
{
    return _variable;
}

void ArrayVariable::declareType(ostream& os) const
{
}

void ArrayVariable::convertType(ostream& os) const
{
}

////////////////////////////////////////
// (source kernel) local memory

LocalMemory::LocalMemory(const size_t PREC,
                         const size_t length)
    : Variable(PREC, 0),
      _length(length) { }

size_t LocalMemory::length(void) const
{
    return _length;
}

void LocalMemory::declareType(ostream& os) const
{
}

void LocalMemory::convertType(ostream& os) const
{
}

////////////////////////////////////////
// (source kernel) scalar argument

ScalarArgument::ScalarArgument(const uint32_t a)
    : Variable(PrecType::UInt32, 0)
{
    _scalar.u = a;
}

ScalarArgument::ScalarArgument(const int32_t a)
    : Variable(PrecType::Int32, 0)
{
    _scalar.i = a;
}

ScalarArgument::ScalarArgument(const float a)
    : Variable(PrecType::Float, 0)
{
    _scalar.f = a;
}

ScalarArgument::ScalarArgument(const double a)
    : Variable(PrecType::Double, 0)
{
    _scalar.d = a;
}

uint32_t ScalarArgument::scalarUInt32(void) const
{
    return _scalar.u;
}

int32_t ScalarArgument::scalarInt32(void) const
{
    return _scalar.i;
}

float ScalarArgument::scalarFloat(void) const
{
    return _scalar.f;
}

double ScalarArgument::scalarDouble(void) const
{
    return _scalar.d;
}

void ScalarArgument::declareType(ostream& os) const
{
}

void ScalarArgument::convertType(ostream& os) const
{
}

}; // namespace chai_internal
