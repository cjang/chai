// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeType.hpp"
#include "CodeVars.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// array and scalar variables

// BaseVar
BaseVar::BaseVar(void)
    : _prec(-1),
      _vecLen(-1),
      _writable(false),
      _variableIdentifier(),
      _inlineValue() { }

BaseVar::BaseVar(const IValue& inlineValue)
    : _prec(-1),
      _vecLen(-1),
      _writable(false),
      _variableIdentifier(),
      _inlineValue(inlineValue) { }

BaseVar::BaseVar(const string& name,
                 const size_t PREC,
                 const size_t vecLen,
                 const bool writable)
    : _prec(PREC),
      _vecLen(vecLen),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue() { }

BaseVar::BaseVar(const string& name,
                 const size_t PREC,
                 const size_t vecLen,
                 const bool writable,
                 const IValue& inlineValue,
                 const bool doInline)
    : _prec(PREC),
      _vecLen(vecLen),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue(doInline
                       ? ConstantValue(inlineValue)
                       : ConstantValue()) { }

BaseVar::BaseVar(const string& name,
                 const size_t PREC,
                 const size_t vecLen,
                 const bool writable,
                 const int inlineValue,
                 const bool doInline)
    : _prec(PREC),
      _vecLen(vecLen),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue(doInline
                       ? ConstantValue(inlineValue)
                       : ConstantValue()) { }

BaseVar& BaseVar::operator= (const BaseVar& other)
{
    _prec               = other._prec;
    _vecLen             = other._vecLen;
    _writable           = other._writable;
    _variableIdentifier = other._variableIdentifier;
    _inlineValue        = other._inlineValue;

    return *this;
}

string BaseVar::name(void) const
{
    if (_inlineValue.empty())
    {
        return _variableIdentifier;
    }
    else
    {
        return _inlineValue.name();
    }
}

// GlobalVar
GlobalVar::GlobalVar(const string& name,
                     const size_t PREC,
                     const size_t vecLen,
                     const bool writable)
    : BaseVar(name,
              PREC,
              vecLen,
              writable) { }

GlobalVar::GlobalVar(const IValue& inlineValue)
    : BaseVar(inlineValue) { }

GlobalVar::GlobalVar(const string& name,
                     const size_t PREC,
                     const size_t vecLen,
                     const bool writable,
                     FunctionDeclaration& funcDecl)
    : BaseVar(name,
              PREC,
              vecLen,
              writable)
{
    funcDecl.argument(*this);
}

GlobalVar::GlobalVar(const string& name,
                     const size_t PREC,
                     const size_t vecLen,
                     const bool writable,
                     FunctionDeclaration& funcDecl,
                     const bool isArgument)
    : BaseVar(name,
              PREC,
              vecLen,
              writable)
{
    if (isArgument)
        funcDecl.argument(*this);
}

string GlobalVar::declaredName(void) const
{
    stringstream ss;

    ss << NameOf::globalvar(_prec, _vecLen, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

size_t GlobalVar::prec(void) const
{
    return _prec;
}

// LocalVar
LocalVar::LocalVar(const string& name,
                   const size_t PREC,
                   const size_t vecLen,
                   const bool writable)
    : BaseVar(name,
              PREC,
              vecLen,
              writable) { }

LocalVar::LocalVar(const IValue& inlineValue)
    : BaseVar(inlineValue) { }

LocalVar::LocalVar(const string& name,
                   const size_t PREC,
                   const size_t vecLen,
                   const bool writable,
                   FunctionDeclaration& funcDecl)
    : BaseVar(name,
              PREC,
              vecLen,
              writable)
{
    funcDecl.argument(*this);
}

LocalVar::LocalVar(const string& name,
                   const size_t PREC,
                   const size_t vecLen,
                   const bool writable,
                   FunctionDeclaration& funcDecl,
                   const bool isArgument)
    : BaseVar(name,
              PREC,
              vecLen,
              writable)
{
    if (isArgument)
        funcDecl.argument(*this);
}

string LocalVar::declaredName(void) const
{
    stringstream ss;

    ss << NameOf::localvar(_prec, _vecLen, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

// PrivateVar
PrivateVar::PrivateVar(void)
    : BaseVar(),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable)
    : BaseVar(name,
              PREC,
              vecLen,
              writable),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const IValue& inlineValue)
    : BaseVar(inlineValue),
     _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       const IValue& inlineValue,
                       const bool doInline)
    : BaseVar(name,
              PREC,
              vecLen,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       const int inlineValue,
                       const bool doInline)
    : BaseVar(name,
              PREC,
              vecLen,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       FunctionDeclaration& funcDecl)
    : BaseVar(name,
              PREC,
              vecLen,
              writable),
      _componentIndex(-1)
{
    funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const bool isArgument)
    : BaseVar(name,
              PREC,
              vecLen,
              writable),
      _componentIndex(-1)
{
    if (isArgument)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const IValue& inlineValue,
                       const bool doInline)
    : BaseVar(name,
              PREC,
              vecLen,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1)
{
    if (! doInline)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const int inlineValue,
                       const bool doInline)
    : BaseVar(name,
              PREC,
              vecLen,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1)
{
    if (! doInline)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t PREC,
                       const size_t vecLen,
                       const bool writable,
                       const size_t componentIndex)
    : BaseVar(name,
              PREC,
              vecLen,
              writable),
      _componentIndex(componentIndex) { }

string PrivateVar::name(void) const
{
    if (-1 == _componentIndex)
    {
        return BaseVar::name();
    }
    else
    {
        stringstream ss;

        ss << BaseVar::name();

        switch (_vecLen)
        {
            case (0) :
            case (2) :
            case (4) :
                ss << ".s" << hex(_componentIndex);
                break;
        }

        return ss.str();
    }
}

string PrivateVar::declaredName(void) const
{
    stringstream ss;

    ss << NameOf::privatevar(_prec, _vecLen, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

size_t PrivateVar::vecLength(void) const
{
    if (0 == _vecLen)
    {
        return PrecType::vecLength(_prec);
    }
    else
    {
        return _vecLen;
    }
}

PrivateVar PrivateVar::operator [] (const size_t componentIndex) const
{
    return PrivateVar(_variableIdentifier,
                      _prec,
                      _vecLen,
                      _writable,
                      componentIndex);
}

// privateVector
vector< PrivateVar > privateVector(const string& name,
                                   const size_t PREC,
                                   const size_t vecLen,
                                   const size_t numberElements)
{
    vector< PrivateVar > vec;

    stringstream ss;

    for (size_t i = 0; i < numberElements; i++)
    {
        ss.str(string());
        ss << name << i;

        vec.push_back(PrivateVar(ss.str(), PREC, vecLen, true));
    }

    return vec;
}

vector< vector< PrivateVar > > privateVector(const string& name,
                                             const size_t PREC,
                                             const size_t vecLen,
                                             const size_t numberElements,
                                             const size_t numberVectors)
{
    vector< vector< PrivateVar > > vecOfVec;

    stringstream ss;

    for (size_t i = 0; i < numberVectors; i++)
    {
        ss.str(string());
        ss << name << i;

        vecOfVec.push_back(
            privateVector(ss.str(), PREC, vecLen, numberElements) );
    }

    return vecOfVec;
}

}; // namespace chai_internal
