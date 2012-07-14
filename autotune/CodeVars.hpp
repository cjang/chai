// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_VARS_HPP_
#define _CHAI_CODE_VARS_HPP_

#include <string>
#include <vector>

#include "CodeFunction.hpp"
#include "CodeInterfaces.hpp"
#include "CodeValues.hpp"

namespace chai_internal {

////////////////////////////////////////
// array and scalar variables

// abstract base class
class BaseVar : public IVariable
{
protected:
    size_t _prec;
    size_t _vecLen;
    bool   _writable;

    std::string   _variableIdentifier;
    ConstantValue _inlineValue;

    // unset
    BaseVar(void);

    // inlined
    BaseVar(const IValue& inlineValue);

    // basic variable
    BaseVar(const std::string& name,
            const size_t PREC,
            const size_t vecLen,
            const bool writable);

    // basic variable may be inlined
    BaseVar(const std::string& name,
            const size_t PREC,
            const size_t vecLen,
            const bool writable,
            const IValue& inlineValue,
            const bool doInline);

    // basic variable may be inlined
    BaseVar(const std::string& name,
            const size_t PREC,
            const size_t vecLen,
            const bool writable,
            const int inlineValue,
            const bool doInline);

    BaseVar& operator= (const BaseVar& other);

public:

    // variable identifier name or inlined value
    std::string name(void) const;
};

// global memory buffers and images
class GlobalVar : public BaseVar
{
public:
    // pointer to global memory
    GlobalVar(const std::string& name,
              const size_t PREC,
              const size_t vecLen,
              const bool writable);

    // inlined pointer to global memory
    GlobalVar(const IValue& inlineValue);

    // function array argument
    GlobalVar(const std::string& name,
              const size_t PREC,
              const size_t vecLen,
              const bool writable,
              FunctionDeclaration& funcDecl);

    // optional function array argument
    GlobalVar(const std::string& name,
              const size_t PREC,
              const size_t vecLen,
              const bool writable,
              FunctionDeclaration& funcDecl,
              const bool isArgument);

    std::string declaredName(void) const;

    size_t prec(void) const;
};

// local memory
class LocalVar : public BaseVar
{
public:
    // pointer to local memory
    LocalVar(const std::string& name,
             const size_t PREC,
             const size_t vecLen,
             const bool writable);

    // inlined pointer to local memory
    LocalVar(const IValue& inlineValue);

    // function local memory argument
    LocalVar(const std::string& name,
             const size_t PREC,
             const size_t vecLen,
             const bool writable,
             FunctionDeclaration& funcDecl);

    // optional function local memory argument
    LocalVar(const std::string& name,
             const size_t PREC,
             const size_t vecLen,
             const bool writable,
             FunctionDeclaration& funcDecl,
             const bool isArgument);

    std::string declaredName(void) const;
};

// private register variables
class PrivateVar : public BaseVar
{
    size_t _componentIndex;

public:
    // unset, needed for vectors of private register variables
    PrivateVar(void);

    // register on stack frame of function
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable);

    // inlined value
    PrivateVar(const IValue& inlineValue);

    // register on stack frame of function, optionally inlined
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               const IValue& inlineValue,
               const bool doInline);

    // register on stack frame of function, optionally inlined
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               const int inlineValue,
               const bool doInline);

    // function scalar argument
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               FunctionDeclaration& funcDec);

    // optional function scalar argument
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               FunctionDeclaration& funcDec,
               const bool isArgument);

    // function scalar argument, optionally inlined
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               FunctionDeclaration& funcDec,
               const IValue& inlineValue,
               const bool doInline);

    // function scalar argument, optionally inlined
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               FunctionDeclaration& funcDec,
               const int inlineValue,
               const bool doInline);

    // component of a private vector element
    PrivateVar(const std::string& name,
               const size_t PREC,
               const size_t vecLen,
               const bool writable,
               const size_t componentIndex);

    std::string name(void) const;
    std::string declaredName(void) const;

    size_t vecLength(void) const;

    // vector type components
    PrivateVar operator[] (const size_t componentIndex) const;
};

std::vector< PrivateVar > privateVector(const std::string& name,
                                        const size_t PREC,
                                        const size_t vecLen,
                                        const size_t numberElements);

std::vector< std::vector< PrivateVar > > privateVector(
                                             const std::string& name,
                                             const size_t PREC,
                                             const size_t vecLen,
                                             const size_t numberElements,
                                             const size_t numberVectors);

}; // namespace chai_internal

#endif
