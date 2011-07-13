// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VARIABLE_HPP_
#define _CHAI_VARIABLE_HPP_

#include <cstddef>
#include <ostream>

#include "Qualifier.hpp"

namespace chai_internal {

////////////////////////////////////////
// variable in a device kernel
//
// const sampler_t     <--- it is always const
// image2d_t           <--- never const
// float|double:1|2|4  <--- may be const, may be pointer
// int                 <--- may be const

// this is never a variable, only reinterpreting type
// uint4               <--- packing doubles in uint32 images

////////////////////////////////////////
// abstract base class for variables
struct Variable
{
    virtual ~Variable(void);

    void identifierName(std::ostream&) const;

    virtual void declareType(std::ostream&) const = 0;
};

////////////////////////////////////////
// image sampler
class Sampler : public Variable
{
public:
    Sampler(void);

    void declareType(std::ostream&) const;
};

////////////////////////////////////////
// image texture
class Image2D : public Variable
{
    const ImageAccess& _imgAccess;

public:
    Image2D(const ImageAccess& rw);

    void declareType(std::ostream&) const;
};

////////////////////////////////////////
// variable declaration
struct ConstVariableDecl { ConstVariableDecl(void); };
struct PointerVariableDecl { PointerVariableDecl(void); };
struct ConstPointerVariableDecl { ConstPointerVariableDecl(void); };
static const ConstVariableDecl CONST;
static const PointerVariableDecl POINTER;
static const ConstPointerVariableDecl CONST_POINTER;

////////////////////////////////////////
// memory buffer
class FloatPt : public Variable
{
    enum Type { SP, DP };
    const Type _fpType;

    const size_t _vectorLength;

    const bool _isConst;
    const bool _isPointer;

    const AddressSpace& _addrSpace;

    Type precisionToType(const size_t precision) const;

public:
    FloatPt(const size_t precision,
            const size_t vectorLength,
            const AddressSpace& qualifier = DEFAULT);

    FloatPt(const size_t precision,
            const size_t vectorLength,
            const ConstVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    FloatPt(const size_t precision,
            const size_t vectorLength,
            const PointerVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    FloatPt(const size_t precision,
            const size_t vectorLength,
            const ConstPointerVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    void declareType(std::ostream&) const;
};

////////////////////////////////////////
// integer scalar

class Integer : public Variable
{
    const bool _isConst;

    const AddressSpace& _addrSpace;

public:
    Integer(const AddressSpace& qualifier = DEFAULT);

    Integer(const ConstVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    void declareType(std::ostream&) const;
};

}; // namespace chai_internal

#endif
