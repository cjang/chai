// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Function.hpp"
#include "PrecType.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// abstract base class for variables

Variable::Variable(void)
    : _func(NULL) { }

Variable::~Variable(void) { }

void Variable::setFunction(Function* funcPtr)
{
    _func = funcPtr;
}

void Variable::identifierName(ostream& os) const
{
    os << _func->varNameLookup(this) << " ";
}

////////////////////////////////////////
// image sampler

Sampler::Sampler(void)
    : Variable() { }

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

Image2D::Image2D(const ImageAccess& rw)
    : Variable(),
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

AddrMem::AddrMem(const size_t precision,
                 const size_t vectorLength,
                 const AddressSpace& qualifier)
    : _precision(precision),
      _vectorLength(vectorLength),
      _isConst(false),
      _isPointer(false),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t precision,
                 const size_t vectorLength,
                 const ConstVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _precision(precision),
      _vectorLength(vectorLength),
      _isConst(true),
      _isPointer(false),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t precision,
                 const size_t vectorLength,
                 const PointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _precision(precision),
      _vectorLength(vectorLength),
      _isConst(false),
      _isPointer(true),
      _addrSpace(qualifier) { }

AddrMem::AddrMem(const size_t precision,
                 const size_t vectorLength,
                 const ConstPointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _precision(precision),
      _vectorLength(vectorLength),
      _isConst(true),
      _isPointer(true),
      _addrSpace(qualifier) { }

bool AddrMem::fp64(void) const
{
    return PrecType::Double == _precision;
}

void AddrMem::declareType(ostream& os) const
{
    os << _addrSpace.str();

    if (_isConst) os << "const ";

    os << PrecType::getPrimitiveName(_precision);

    if (_vectorLength > 1) os << _vectorLength;

    os << (_isPointer ? " * " : " ");
}

void AddrMem::convertType(ostream& os) const
{
    os << "(";

    os << PrecType::getPrimitiveName(_precision);

    if (_vectorLength > 1) os << _vectorLength;

    os << ")";
}

////////////////////////////////////////
// integer scalar

Integer::Integer(const AddressSpace& qualifier)
    : _isConst(false),
      _addrSpace(qualifier) { }

Integer::Integer(const ConstVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _isConst(true),
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

}; // namespace chai_internal
