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
    : _precision(-1),
      _vectorLength(-1),
      _writable(false),
      _variableIdentifier(),
      _inlineValue() { }

BaseVar::BaseVar(const IValue& inlineValue)
    : _precision(-1),
      _vectorLength(-1),
      _writable(false),
      _variableIdentifier(),
      _inlineValue(inlineValue) { }

BaseVar::BaseVar(const string& name,
                 const size_t precision,
                 const size_t vectorLength,
                 const bool writable)
    : _precision(precision),
      _vectorLength(vectorLength),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue() { }

BaseVar::BaseVar(const string& name,
                 const size_t precision,
                 const size_t vectorLength,
                 const bool writable,
                 const IValue& inlineValue,
                 const bool doInline)
    : _precision(precision),
      _vectorLength(vectorLength),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue(doInline
                       ? ConstantValue(inlineValue)
                       : ConstantValue()) { }

BaseVar::BaseVar(const string& name,
                 const size_t precision,
                 const size_t vectorLength,
                 const bool writable,
                 const int inlineValue,
                 const bool doInline)
    : _precision(precision),
      _vectorLength(vectorLength),
      _writable(writable),
      _variableIdentifier(name),
      _inlineValue(doInline
                       ? ConstantValue(inlineValue)
                       : ConstantValue()) { }

BaseVar& BaseVar::operator= (const BaseVar& other)
{
    _precision          = other._precision;
    _vectorLength       = other._vectorLength;
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
                     const size_t precision,
                     const size_t vectorLength,
                     const bool writable)
    : BaseVar(name,
              precision,
              vectorLength,
              writable) { }

GlobalVar::GlobalVar(const IValue& inlineValue)
    : BaseVar(inlineValue) { }

GlobalVar::GlobalVar(const string& name,
                     const size_t precision,
                     const size_t vectorLength,
                     const bool writable,
                     FunctionDeclaration& funcDecl)
    : BaseVar(name,
              precision,
              vectorLength,
              writable)
{
    funcDecl.argument(*this);
}

GlobalVar::GlobalVar(const string& name,
                     const size_t precision,
                     const size_t vectorLength,
                     const bool writable,
                     FunctionDeclaration& funcDecl,
                     const bool isArgument)
    : BaseVar(name,
              precision,
              vectorLength,
              writable)
{
    if (isArgument)
        funcDecl.argument(*this);
}

string GlobalVar::declaredName(void) const
{
    stringstream ss;

    ss << NameOf::globalvar(_precision, _vectorLength, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

size_t GlobalVar::precision(void) const
{
    return _precision;
}

// LocalVar
LocalVar::LocalVar(const string& name,
                   const size_t precision,
                   const size_t vectorLength,
                   const bool writable)
    : BaseVar(name,
              precision,
              vectorLength,
              writable) { }

LocalVar::LocalVar(const IValue& inlineValue)
    : BaseVar(inlineValue) { }

LocalVar::LocalVar(const string& name,
                   const size_t precision,
                   const size_t vectorLength,
                   const bool writable,
                   FunctionDeclaration& funcDecl)
    : BaseVar(name,
              precision,
              vectorLength,
              writable)
{
    funcDecl.argument(*this);
}

LocalVar::LocalVar(const string& name,
                   const size_t precision,
                   const size_t vectorLength,
                   const bool writable,
                   FunctionDeclaration& funcDecl,
                   const bool isArgument)
    : BaseVar(name,
              precision,
              vectorLength,
              writable)
{
    if (isArgument)
        funcDecl.argument(*this);
}

string LocalVar::declaredName(void) const
{
    stringstream ss;

    ss << NameOf::localvar(_precision, _vectorLength, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

// PrivateVar
PrivateVar::PrivateVar(void)
    : BaseVar(),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable)
    : BaseVar(name,
              precision,
              vectorLength,
              writable),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const IValue& inlineValue)
    : BaseVar(inlineValue),
     _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       const IValue& inlineValue,
                       const bool doInline)
    : BaseVar(name,
              precision,
              vectorLength,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       const int inlineValue,
                       const bool doInline)
    : BaseVar(name,
              precision,
              vectorLength,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1) { }

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       FunctionDeclaration& funcDecl)
    : BaseVar(name,
              precision,
              vectorLength,
              writable),
      _componentIndex(-1)
{
    funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const bool isArgument)
    : BaseVar(name,
              precision,
              vectorLength,
              writable),
      _componentIndex(-1)
{
    if (isArgument)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const IValue& inlineValue,
                       const bool doInline)
    : BaseVar(name,
              precision,
              vectorLength,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1)
{
    if (! doInline)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       FunctionDeclaration& funcDecl,
                       const int inlineValue,
                       const bool doInline)
    : BaseVar(name,
              precision,
              vectorLength,
              writable,
              inlineValue,
              doInline),
      _componentIndex(-1)
{
    if (! doInline)
        funcDecl.argument(*this);
}

PrivateVar::PrivateVar(const string& name,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable,
                       const size_t componentIndex)
    : BaseVar(name,
              precision,
              vectorLength,
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

        switch (_vectorLength)
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

    ss << NameOf::privatevar(_precision, _vectorLength, _writable)
       << " "
       << _variableIdentifier;

    return ss.str();
}

size_t PrivateVar::vectorLength(void) const
{
    if (0 == _vectorLength)
    {
        return PrecType::vecLength(_precision);
    }
    else
    {
         return _vectorLength;
    }
}

PrivateVar PrivateVar::operator [] (const size_t componentIndex) const
{
    return PrivateVar(_variableIdentifier,
                      _precision,
                      _vectorLength,
                      _writable,
                      componentIndex);
}

// privateVector
vector< PrivateVar > privateVector(const string& name,
                                   const size_t precision,
                                   const size_t vectorLength,
                                   const size_t numberElements)
{
    vector< PrivateVar > vec;

    stringstream ss;

    for (size_t i = 0; i < numberElements; i++)
    {
        ss.str(string());
        ss << name << i;

        vec.push_back(PrivateVar(ss.str(), precision, vectorLength, true));
    }

    return vec;
}

vector< vector< PrivateVar > > privateVector(const string& name,
                                             const size_t precision,
                                             const size_t vectorLength,
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
            privateVector(ss.str(), precision, vectorLength, numberElements) );
    }

    return vecOfVec;
}

}; // namespace chai_internal
