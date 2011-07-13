// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// abstract base class for variables

Variable::~Variable(void) { }

void Variable::identifierName(ostream& os) const
{
    os << "var" << this << " ";
}

////////////////////////////////////////
// image sampler

Sampler::Sampler(void)
    : Variable() { }

void Sampler::declareType(ostream& os) const
{
    os << "const sampler_t ";
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

////////////////////////////////////////
// variable declaration

ConstVariableDecl::ConstVariableDecl(void) { }
PointerVariableDecl::PointerVariableDecl(void) { }
ConstPointerVariableDecl::ConstPointerVariableDecl(void) { }

////////////////////////////////////////
// memory buffer

FloatPt::Type FloatPt::precisionToType(const size_t precision) const
{
    return sizeof(double) == precision ? DP : SP;
}

FloatPt::FloatPt(const size_t precision,
                 const size_t vectorLength,
                 const AddressSpace& qualifier)
    : _fpType(precisionToType(precision)),
      _vectorLength(vectorLength),
      _isConst(false),
      _isPointer(false),
      _addrSpace(qualifier) { }

FloatPt::FloatPt(const size_t precision,
                 const size_t vectorLength,
                 const ConstVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _fpType(precisionToType(precision)),
      _vectorLength(vectorLength),
      _isConst(true),
      _isPointer(false),
      _addrSpace(qualifier) { }

FloatPt::FloatPt(const size_t precision,
                 const size_t vectorLength,
                 const PointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _fpType(precisionToType(precision)),
      _vectorLength(vectorLength),
      _isConst(false),
      _isPointer(true),
      _addrSpace(qualifier) { }

FloatPt::FloatPt(const size_t precision,
                 const size_t vectorLength,
                 const ConstPointerVariableDecl& varDecl,
                 const AddressSpace& qualifier)
    : _fpType(precisionToType(precision)),
      _vectorLength(vectorLength),
      _isConst(true),
      _isPointer(true),
      _addrSpace(qualifier) { }

void FloatPt::declareType(ostream& os) const
{
    os << _addrSpace.str();

    if (_isConst) os << "const ";

    switch (_fpType)
    {
        case (SP) :
            os << "float";
            break;

        case (DP) :
            os << "double";
            break;
    }

    if (_vectorLength > 1) os << _vectorLength;

    os << (_isPointer ? " * " : " ");
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

}; // namespace chai_internal
