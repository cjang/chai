// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VARIABLE_HPP_
#define _CHAI_VARIABLE_HPP_

#include <cstddef>
#include <ostream>

#include "Qualifier.hpp"

namespace chai_internal {

class Function;

////////////////////////////////////////
// variable in a device kernel
//
// For synthesized kernels:
//
//   const sampler_t     <--- it is always const
//   image2d_t           <--- never const
//   float|double:1|2|4  <--- may be const, may be pointer
//   int                 <--- may be const
//
//   this is never a variable, only reinterpreting type
//   uint4               <--- packing doubles in uint32 images
//
// For source kernels:
//
//   array variable object, may be image or memory buffer
//   local memory
//   scalar argument
//

////////////////////////////////////////
// abstract base class for variables
class Variable
{
    const size_t _prec;
    const size_t _vecLen;

    Function* _func;

public:
    Variable(const size_t PREC,
             const size_t vecLen);
    virtual ~Variable(void);

    void setFunction(Function*);

    void identifierName(std::ostream&) const;
    size_t prec(void) const;
    size_t vecLength(void) const;

    virtual void declareType(std::ostream&) const = 0;
    virtual void convertType(std::ostream&) const = 0;
};

////////////////////////////////////////
// image sampler
class Sampler : public Variable
{
public:
    Sampler(void);

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
};

////////////////////////////////////////
// image texture
class Image2D : public Variable
{
    const ImageAccess& _imgAccess;

public:
    Image2D(const size_t PREC,
            const size_t vecLen,
            const ImageAccess& rw);

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
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
// memory buffer (addressable memory)
class AddrMem : public Variable
{
    const bool _isConst;
    const bool _isPointer;

    const AddressSpace& _addrSpace;

public:
    AddrMem(const size_t PREC,
            const size_t vecLen,
            const AddressSpace& qualifier = DEFAULT);

    AddrMem(const size_t PREC,
            const size_t vecLen,
            const ConstVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    AddrMem(const size_t PREC,
            const size_t vecLen,
            const PointerVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    AddrMem(const size_t PREC,
            const size_t vecLen,
            const ConstPointerVariableDecl& varDecl,
            const AddressSpace& qualifier = DEFAULT);

    bool fp64(void) const;

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
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
    void convertType(std::ostream&) const;
};

////////////////////////////////////////
// (source kernel) array variable
class ArrayVariable : public Variable
{
    const uint32_t _variable;

public:
    ArrayVariable(const size_t PREC,
                  const uint32_t varNum);

    uint32_t variable(void) const;

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
};

////////////////////////////////////////
// (source kernel) local memory
class LocalMemory : public Variable
{
    const size_t _length;

public:
    LocalMemory(const size_t PREC,
                const size_t length);

    size_t length(void) const;

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
};

////////////////////////////////////////
// (source kernel) scalar argument

class ScalarArgument : public Variable
{
    union
    {
        uint32_t u;
        int32_t  i;
        float    f;
        double   d;
    } _scalar;

public:
    ScalarArgument(const uint32_t);
    ScalarArgument(const int32_t);
    ScalarArgument(const float);
    ScalarArgument(const double);

    uint32_t scalarUInt32(void) const;
    int32_t scalarInt32(void) const;
    float scalarFloat(void) const;
    double scalarDouble(void) const;

    void declareType(std::ostream&) const;
    void convertType(std::ostream&) const;
};

}; // namespace chai_internal

#endif
