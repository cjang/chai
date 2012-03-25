// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_VALUES_HPP_
#define _CHAI_CODE_VALUES_HPP_

#include <ostream>
#include <string>

#include "CodeInterfaces.hpp"

namespace chai_internal {

////////////////////////////////////////
// constants and value transformations

class ConstantValue : public IValue
{
    std::string _value;

    std::string str(const int value) const;
    std::string str(const size_t value) const;

public:
    ConstantValue(void);
    ConstantValue(const int value);
    ConstantValue(const size_t value);
    ConstantValue(const std::string& value);
    ConstantValue(const IValue& value);

    ConstantValue& operator= (const ConstantValue& other);

    bool empty(void) const;

    std::string name(void) const;
};

class CastValue : public IValue
{
    const size_t  _precision;
    const size_t  _vectorLength;
    const IValue& _value;

public:
    CastValue(const IValue& value,
              const size_t precision,
              const size_t vectorLength);

    std::string name(void) const;
};

class ReinterpretValue : public IValue
{
    const size_t  _precision;
    const size_t  _vectorLength;
    const IValue& _value;
    const bool    _doApply;

public:
    ReinterpretValue(const IValue& value,
                     const size_t precision,
                     const size_t vectorLength,
                     const bool doApply = true);

    std::string name(void) const;
};

class ConvertValue : public IValue
{
    const size_t  _precision;
    const size_t  _vectorLength;
    const IValue& _value;
    const bool    _doApply;

public:
    ConvertValue(const IValue& value,
                 const size_t precision,
                 const size_t vectorLength,
                 const bool doApply = true);

    std::string name(void) const;
};

class DerefValue : public IValue
{
    const IValue& _value;

public:
    DerefValue(const IValue& value);

    std::string name() const;
};

ConstantValue operator* (const IValue& right);

class PostIncValue : public IValue
{
    const IValue& _value;

public:
    PostIncValue(const IValue& value);

    std::string name(void) const;
};

ConstantValue operator++ (const IValue& left, int dummy);

class BinOpValue : public IValue
{
    const ConstantValue _leftNum;
    const ConstantValue _rightNum;
    const IValue&       _left;
    const IValue&       _right;

protected:
    std::string left(void) const;
    std::string right(void) const;
    std::string total(const std::string& op) const;

    BinOpValue(const IValue& left, const IValue& right);
    BinOpValue(const IValue& left, const size_t right);
    BinOpValue(const size_t left, const IValue& right);

public:
    virtual std::string name(void) const = 0;
};

struct AddValue : public BinOpValue
{
    AddValue(const IValue& left, const IValue& right);
    AddValue(const IValue& left, const size_t right);
    AddValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct SubValue : public BinOpValue
{
    SubValue(const IValue& left, const IValue& right);
    SubValue(const IValue& left, const size_t right);
    SubValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct MulValue : public BinOpValue
{
    MulValue(const IValue& left, const IValue& right);
    MulValue(const IValue& left, const size_t right);
    MulValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct DivValue : public BinOpValue
{
    DivValue(const IValue& left, const IValue& right);
    DivValue(const IValue& left, const size_t right);
    DivValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct ModValue : public BinOpValue
{
    ModValue(const IValue& left, const IValue& right);
    ModValue(const IValue& left, const size_t right);
    ModValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct RightShiftValue : public BinOpValue
{
    RightShiftValue(const IValue& left, const IValue& right);
    RightShiftValue(const IValue& left, const size_t right);
    RightShiftValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

struct LeftShiftValue : public BinOpValue
{
    LeftShiftValue(const IValue& left, const IValue& right);
    LeftShiftValue(const IValue& left, const size_t right);
    LeftShiftValue(const size_t left, const IValue& right);

    std::string name(void) const;
};

ConstantValue operator+ (const IValue& left, const IValue& right);
ConstantValue operator+ (const IValue& left, const size_t right);
ConstantValue operator+ (const size_t left, const IValue& right);

ConstantValue operator- (const IValue& left, const IValue& right);
ConstantValue operator- (const IValue& left, const size_t right);
ConstantValue operator- (const size_t left, const IValue& right);

ConstantValue operator* (const IValue& left, const IValue& right);
ConstantValue operator* (const IValue& left, const size_t right);
ConstantValue operator* (const size_t left, const IValue& right);

ConstantValue operator/ (const IValue& left, const IValue& right);
ConstantValue operator/ (const IValue& left, const size_t right);
ConstantValue operator/ (const size_t left, const IValue& right);

ConstantValue operator% (const IValue& left, const IValue& right);
ConstantValue operator% (const IValue& left, const size_t right);
ConstantValue operator% (const size_t left, const IValue& right);

ConstantValue operator>> (const IValue& left, const IValue& right);
ConstantValue operator>> (const IValue& left, const size_t right);
ConstantValue operator>> (const size_t left, const IValue& right);

ConstantValue operator<< (const IValue& left, const IValue& right);
ConstantValue operator<< (const IValue& left, const size_t right);
ConstantValue operator<< (const size_t left, const IValue& right);

// MAD operation
class MADValue : public IValue
{
    const ConstantValue _as;
    const ConstantValue _bs;
    const ConstantValue _cs;

    const IValue& _a;
    const IValue& _b;
    const IValue& _c;

    const bool _useMADFunction;

public:
    MADValue(const IValue& a,
             const IValue& b,
             const IValue& c,
             const bool useMADFunction = true);

    MADValue(const std::string& a,
             const std::string& b,
             const std::string& c,
             const bool useMADFunction = true);

    MADValue(const std::string& a,
             const std::string& b,
             const IValue& c,
             const bool useMADFunction = true);

    std::string name(void) const;
};

}; // namespace chai_internal

#endif
