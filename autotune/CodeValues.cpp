// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeType.hpp"
#include "CodeValues.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// constants and value transformations

// ConstantValue
string ConstantValue::str(const int value) const
{
    stringstream ss;

    ss << value;

    return ss.str();
}

string ConstantValue::str(const size_t value) const
{
    stringstream ss;

    ss << value;

    return ss.str();
}

ConstantValue::ConstantValue(void)
    : _value() { }

ConstantValue::ConstantValue(const int value)
    : _value(str(value)) { }

ConstantValue::ConstantValue(const size_t value)
    : _value(str(value)) { }

ConstantValue::ConstantValue(const string& value)
    : _value(value) { }

ConstantValue::ConstantValue(const IValue& value)
    : _value(value.name()) { }

ConstantValue& ConstantValue::operator= (const ConstantValue& other)
{
    _value = other._value;

    return *this;
}

bool ConstantValue::empty(void) const
{
    return _value.empty();
}

string ConstantValue::name(void) const
{
    stringstream ss;

    ss << _value;

    return ss.str();
}

// CastValue
CastValue::CastValue(const IValue& value,
                     const size_t precision,
                     const size_t vectorLength)
    : _precision(precision),
      _vectorLength(vectorLength),
      _value(value) { }

string CastValue::name(void) const
{
    stringstream ss;

    ss << "("
       << NameOf::privatevar(_precision, _vectorLength, true)
       << ")("
       << _value.name()
       << ")";

    return ss.str();
}

// ReinterpretValue
ReinterpretValue::ReinterpretValue(const IValue& value,
                                   const size_t precision,
                                   const size_t vectorLength,
                                   const bool doApply)
    : _precision(precision),
      _vectorLength(vectorLength),
      _value(value),
      _doApply(doApply) { }

string ReinterpretValue::name(void) const
{
    stringstream ss;

    if (_doApply)
    {
        ss << "as_"
           << NameOf::privatevar(_precision, _vectorLength, true)
           << "("
           << _value.name()
           << ")";
    }
    else
    {
        ss << _value.name();
    }

    return ss.str();
}

// ConvertValue
ConvertValue::ConvertValue(const IValue& value,
                           const size_t precision,
                           const size_t vectorLength,
                           const bool doApply)
    : _precision(precision),
      _vectorLength(vectorLength),
      _value(value),
      _doApply(doApply) { }

string ConvertValue::name(void) const
{
    stringstream ss;

    if (_doApply)
    {
        ss << "convert_"
           << NameOf::privatevar(_precision, _vectorLength, true)
           << "("
           << _value.name()
           << ")";
    }
    else
    {
        ss << _value.name();
    }

    return ss.str();
}

// DerefValue
DerefValue::DerefValue(const IValue& value)
    : _value(value) { }

string DerefValue::name() const
{
    stringstream ss;

    ss << "*" << _value.name();

    return ss.str();
}

ConstantValue operator* (const IValue& right)
{
    return DerefValue(right);
}

// PostIncValue
PostIncValue::PostIncValue(const IValue& value)
    : _value(value) { }

string PostIncValue::name(void) const
{
    stringstream ss;

    ss << _value.name() << "++";

    return ss.str();
}

ConstantValue operator++ (const IValue& left,
                          int dummy)
{
    return PostIncValue(left);
}

// BinOpValue
string BinOpValue::left(void) const
{
    return _left.name();
}

string BinOpValue::right(void) const
{
    return _right.name();
}

string BinOpValue::total(const string& op) const
{
    stringstream ss;

    ss << "(" << left() << op << right() << ")";

    return ss.str();
}

BinOpValue::BinOpValue(const IValue& left,
                       const IValue& right)
    : _leftNum(),
      _rightNum(),
      _left(left),
      _right(right) { }

BinOpValue::BinOpValue(const IValue& left,
                       const size_t right)
    : _leftNum(),
      _rightNum(right),
      _left(left),
      _right(_rightNum) { }

BinOpValue::BinOpValue(const size_t left,
                       const IValue& right)
    : _leftNum(left),
      _rightNum(),
      _left(_leftNum),
      _right(right) { }

// AddValue
AddValue::AddValue(const IValue& left,
                   const IValue& right)
    : BinOpValue(left, right) { }

AddValue::AddValue(const IValue& left,
                   const size_t right)
    : BinOpValue(left, right) { }

AddValue::AddValue(const size_t left,
                   const IValue& right)
    : BinOpValue(left, right) { }

string AddValue::name(void) const
{
    if ("0" == left())
    {
        return right();
    }
    else if ("0" == right())
    {
        return left();
    }
    else
    {
        return total(" + ");
    }
}

// SubValue
SubValue::SubValue(const IValue& left,
                   const IValue& right)
    : BinOpValue(left, right) { }

SubValue::SubValue(const IValue& left,
                   const size_t right)
    : BinOpValue(left, right) { }

SubValue::SubValue(const size_t left,
                   const IValue& right)
    : BinOpValue(left, right) { }

string SubValue::name(void) const
{
    if ("0" == right())
    {
        return left();
    }
    else
    {
        return total(" - ");
    }
}

// MulValue
MulValue::MulValue(const IValue& left,
                   const IValue& right)
    : BinOpValue(left, right) { }

MulValue::MulValue(const IValue& left,
                   const size_t right)
    : BinOpValue(left, right) { }

MulValue::MulValue(const size_t left,
                   const IValue& right)
    : BinOpValue(left, right) { }

string MulValue::name(void) const
{
    if ("1" == left())
    {
        return right();
    }
    else if ("1" == right())
    {
        return left();
    }
    else if ("0" == left())
    {
        return left();
    }
    else if ("0" == right())
    {
        return right();
    }
    else
    {
        return total(" * ");
    }
}

// DivValue
DivValue::DivValue(const IValue& left,
                   const IValue& right)
    : BinOpValue(left, right) { }

DivValue::DivValue(const IValue& left,
                   const size_t right)
    : BinOpValue(left, right) { }

DivValue::DivValue(const size_t left,
                   const IValue& right)
    : BinOpValue(left, right) { }

string DivValue::name(void) const
{
    if ("1" == right())
    {
        return left();
    }
    else if ("0" == left())
    {
        return left();
    }
    else
    {
        return total(" / ");
    }
}

// ModValue
ModValue::ModValue(const IValue& left,
                   const IValue& right)
    : BinOpValue(left, right) { }

ModValue::ModValue(const IValue& left,
                   const size_t right)
    : BinOpValue(left, right) { }

ModValue::ModValue(const size_t left,
                   const IValue& right)
    : BinOpValue(left, right) { }

string ModValue::name(void) const
{
    return total(" % ");
}

// RightShiftValue
RightShiftValue::RightShiftValue(const IValue& left,
                                 const IValue& right)
    : BinOpValue(left, right) { }

RightShiftValue::RightShiftValue(const IValue& left,
                                 const size_t right)
    : BinOpValue(left, right) { }

RightShiftValue::RightShiftValue(const size_t left,
                                 const IValue& right)
    : BinOpValue(left, right) { }

string RightShiftValue::name(void) const
{
    return total(" >> ");
}

// LeftShiftValue
LeftShiftValue::LeftShiftValue(const IValue& left,
                               const IValue& right)
    : BinOpValue(left, right) { }

LeftShiftValue::LeftShiftValue(const IValue& left,
                               const size_t right)
    : BinOpValue(left, right) { }

LeftShiftValue::LeftShiftValue(const size_t left,
                               const IValue& right)
    : BinOpValue(left, right) { }

string LeftShiftValue::name(void) const
{
    return total(" << ");
}

// overloaded operators
ConstantValue operator+ (const IValue& left,
                         const IValue& right)
{
    return AddValue(left, right);
}

ConstantValue operator+ (const IValue& left,
                         const size_t right)
{
    return AddValue(left, right);
}

ConstantValue operator+ (const size_t left,
                         const IValue& right)
{
    return AddValue(left, right);
}

ConstantValue operator- (const IValue& left,
                         const IValue& right)
{
    return SubValue(left, right);
}

ConstantValue operator- (const IValue& left,
                         const size_t right)
{
    return SubValue(left, right);
}

ConstantValue operator- (const size_t left,
                         const IValue& right)
{
    return SubValue(left, right);
}

ConstantValue operator* (const IValue& left,
                         const IValue& right)
{
    return MulValue(left, right);
}

ConstantValue operator* (const IValue& left,
                         const size_t right)
{
    return MulValue(left, right);
}

ConstantValue operator* (const size_t left,
                         const IValue& right)
{
    return MulValue(left, right);
}

ConstantValue operator/ (const IValue& left,
                         const IValue& right)
{
    return DivValue(left, right);
}

ConstantValue operator/ (const IValue& left,
                         const size_t right)
{
    return DivValue(left, right);
}

ConstantValue operator/ (const size_t left,
                         const IValue& right)
{
    return DivValue(left, right);
}

ConstantValue operator% (const IValue& left,
                         const IValue& right)
{
    return ModValue(left, right);
}

ConstantValue operator% (const IValue& left,
                         const size_t right)
{
    return ModValue(left, right);
}

ConstantValue operator% (const size_t left,
                         const IValue& right)
{
    return ModValue(left, right);
}

ConstantValue operator>> (const IValue& left,
                          const IValue& right)
{
    return RightShiftValue(left, right);
}

ConstantValue operator>> (const IValue& left,
                          const size_t right)
{
    return RightShiftValue(left, right);
}

ConstantValue operator>> (const size_t left,
                          const IValue& right)
{
    return RightShiftValue(left, right);
}

ConstantValue operator<< (const IValue& left,
                          const IValue& right)
{
    return LeftShiftValue(left, right);
}

ConstantValue operator<< (const IValue& left,
                          const size_t right)
{
    return LeftShiftValue(left, right);
}

ConstantValue operator<< (const size_t left,
                          const IValue& right)
{
    return LeftShiftValue(left, right);
}

// MADValue
MADValue::MADValue(const IValue& a,
                   const IValue& b,
                   const IValue& c,
                   const bool useMADFunction)
    : _as(),
      _bs(),
      _cs(),
      _a(a),
      _b(b),
      _c(c),
      _useMADFunction(useMADFunction) { }

MADValue::MADValue(const string& a,
                   const string& b,
                   const string& c,
                   const bool useMADFunction)
    : _as(a),
      _bs(b),
      _cs(c),
      _a(_as),
      _b(_bs),
      _c(_cs),
      _useMADFunction(useMADFunction) { }

MADValue::MADValue(const string& a,
                   const string& b,
                   const IValue& c,
                   const bool useMADFunction)
    : _as(a),
      _bs(b),
      _cs(),
      _a(_as),
      _b(_bs),
      _c(c),
      _useMADFunction(useMADFunction) { }

string MADValue::name(void) const
{
    stringstream ss;

    if (_useMADFunction)
    {
        ss << "mad("
           << _a.name()
           << ", "
           << _b.name()
           << ", "
           << _c.name()
           << ")";
    }
    else
    {
        ss << "("
           << _a.name()
           << " * "
           << _b.name()
           << " + "
           << _c.name()
           << ")";
    }

    return ss.str();
}

}; // namespace chai_internal
