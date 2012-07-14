// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstScalar.hpp"
#include "PrecType.hpp"
#include "TransScalar.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatched operation

BaseAst* TransScalar::sub_eval(void) const
{
    switch (_prec)
    {
        case (PrecType::UInt32) :
            return new AstScalar(static_cast<uint32_t>(_argScalar[0]));

        case (PrecType::Int32) :
            return new AstScalar(static_cast<int32_t>(_argScalar[0]));

        case (PrecType::Float) :
            return new AstScalar(static_cast<float>(_argScalar[0]));

        case (PrecType::Double) :
            return new AstScalar(static_cast<double>(_argScalar[0]));
    }
}

TransScalar::TransScalar(const size_t PREC)
    : BaseTrans(1, 0),
      _prec(PREC) { }

}; // namespace chai_internal
