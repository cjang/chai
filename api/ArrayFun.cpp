// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// single argument operation

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); }

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const ArrayExp& a2,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); arg(a2); }

ArrayFun::ArrayFun(const uint32_t f,
                   const ArrayExp& a,
                   const ArrayExp& a2,
                   const ArrayExp& a3,
                   const DimStack d)
    : _funCode(f),
      _dimStack(d) { arg(a); arg(a2); arg(a3); }

ArrayFun::ArrayFun(const ArrayFun& other)
    : _funCode(other._funCode),
      _dimStack(other._dimStack),
      _argList(other._argList) { }

uint32_t ArrayFun::code(void) const { return _funCode; }

void ArrayFun::arg(const ArrayExp& a) { _argList.push_back(&a); }

void ArrayFun::accept(Stak<BC>& v) const
{
    for (vector<const ArrayExp*>::const_reverse_iterator
         it = _argList.rbegin();
         it != _argList.rend();
         it++) { (*it)->accept(v); }

    v.push(_funCode);
}

void ArrayFun::accept(stack< ArrayDim >& v) const
{
    for (vector<const ArrayExp*>::const_reverse_iterator
         it = _argList.rbegin();
         it != _argList.rend();
         it++) { (*it)->accept(v); }

    switch (_dimStack)
    {
        case (UNCHANGED) :
            break;

        case (REDUCED) :
            {
            ArrayDim a(1, 1, v.top().slots); // reduction to scalar
            v.pop();
            v.push(a);
            }
            break;

        case (MATMUL) :
            {
            const ArrayDim l = v.top();
            v.pop();
            const ArrayDim r = v.top();
            v.pop();
            const size_t maxslots = l.slots > r.slots ? l.slots : r.slots;
            if (1 == l.height && 1 == r.height)
                v.push(ArrayDim(r.width, l.width, maxslots));  // outer product
            else if (1 == l.height)
                v.push(ArrayDim(r.width, 1, maxslots));        // vector-matrix
            else if (1 == r.height)
                v.push(ArrayDim(l.height, 1, maxslots));       // matrix-vector
            else
                v.push(ArrayDim(r.width, l.height, maxslots)); // matrix-matrix
            }
            break;

        case (FIRST2) :
            {
            const ArrayDim a = v.top(); // first argument of two
            v.pop();
            v.pop();
            v.push(a);
            }
            break;

        case (FIRST3) :
            {
            const ArrayDim a = v.top(); // first argument of three
            v.pop();
            v.pop();
            v.pop();
            v.push(a);
            }
            break;

        case (TRANSPOSED) :
            {
            const ArrayDim s = v.top();
            v.pop();
            v.push(ArrayDim(s.height, s.width, s.slots)); // swap width, height
            }
            break;
    }
}

}; // namespace chai
