// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstLitdata.hpp"
#include "PrecType.hpp"
#include "TransLitdata.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransLitdata::sub_eval(void) const
{
    if (1 == _N)
    {
        switch (_precision)
        {
            case (PrecType::UInt32) :
                return new AstLitdata(_uintValue, _argScalar[0]);
            case (PrecType::Int32) :
                return new AstLitdata(_intValue, _argScalar[0]);
            case (PrecType::Float) :
                return new AstLitdata(_floatValue, _argScalar[0]);
            case (PrecType::Double) :
                return new AstLitdata(_doubleValue, _argScalar[0]);
        }
    }
    else
    {
        switch (_precision)
        {
            case (PrecType::UInt32) :
                return new AstLitdata(_uintValue,
                                      _argScalar[0], _argScalar[1]);
            case (PrecType::Int32) :
                return new AstLitdata(_intValue,
                                      _argScalar[0], _argScalar[1]);
            case (PrecType::Float) :
                return new AstLitdata(_floatValue,
                                      _argScalar[0], _argScalar[1]);
            case (PrecType::Double) :
                return new AstLitdata(_doubleValue,
                                      _argScalar[0], _argScalar[1]);
        }
    }
}

TransLitdata::TransLitdata(const size_t N, const uint32_t uintValue)
    : BaseTrans(N, 0),
      _N(N),
      _precision(PrecType::UInt32),
      _uintValue(uintValue),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0) { }

TransLitdata::TransLitdata(const size_t N, const int32_t intValue)
    : BaseTrans(N, 0),
      _N(N),
      _precision(PrecType::Int32),
      _uintValue(0),
      _intValue(intValue),
      _floatValue(0),
      _doubleValue(0) { }

TransLitdata::TransLitdata(const size_t N, const float floatValue)
    : BaseTrans(N, 0),
      _N(N),
      _precision(PrecType::Float),
      _uintValue(0),
      _intValue(0),
      _floatValue(floatValue),
      _doubleValue(0) { }

TransLitdata::TransLitdata(const size_t N, const double doubleValue)
    : BaseTrans(N, 0),
      _N(N),
      _precision(PrecType::Double),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(doubleValue) { }

}; // namespace chai_internal
