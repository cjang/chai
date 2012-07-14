// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array expression interface

ArrayDim::ArrayDim(const size_t w, const size_t h)
    : width(w),
      height(h),
      slots(1) { }

ArrayDim::ArrayDim(const size_t w, const size_t h, const size_t s)
    : width(w),
      height(h),
      slots(s) { }

ArrayExp::~ArrayExp(void) { }

}; // namespace chai
