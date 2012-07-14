// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// special function for index creation

IndexFun::IndexFun(const uint32_t f,
                   const size_t indexDim,
                   const size_t W,
                   const size_t H)
    : _funCode(f),
      _pushH(ByteCodes::index2_f32 == f || ByteCodes::index2_f64 == f),
      _indexDim(indexDim),
      _W(W),
      _H(H) { }

void IndexFun::accept(Stak<BC>& v) const
{
    if (_pushH)
        v.push(static_cast<double>(_H));

    v.push(static_cast<double>(_W));
    v.push(static_cast<double>(_indexDim));

    v.push(_funCode);
}

void IndexFun::accept(stack< ArrayDim >& v) const
{
    v.push(ArrayDim(_W, _H));
}

#define DEFN_INDEX_FUN(FP) \
IndexFun index_ ## FP (const size_t indexDim, const size_t W) \
{ \
    return IndexFun(ByteCodes::index1_ ## FP, indexDim, W, 1); \
} \
IndexFun index_ ## FP (const size_t indexDim, const size_t W, const size_t H) \
{ \
    return IndexFun(ByteCodes::index2_ ## FP, indexDim, W, H); \
}

DEFN_INDEX_FUN(u32)
DEFN_INDEX_FUN(i32)
DEFN_INDEX_FUN(f32)
DEFN_INDEX_FUN(f64)

}; // namespace chai
