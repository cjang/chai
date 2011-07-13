// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_MATMUL_HPP_
#define _CHAI_INTERP_MATMUL_HPP_

#include "BaseInterp.hpp"
#include "UtilFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// matmul

class InterpMatmul : public BaseInterp
{
    template <typename D0, typename D1, typename X>
    void vecvec(const D0* d0, const D1* d1, X* dOut) const
    {
        for (size_t x = 0; x < W(1); x++)
        for (size_t y = 0; y < W(0); y++)
            dOut[x + y * W(1)] = d0 [y] * d1 [x];
    }

    template <typename D0, typename D1, typename X>
    void vecmat(const D0* d0, const D1* d1, X* dOut) const
    {
        const size_t maxK = max<size_t>(W(0), H(1));
        for (size_t x = 0; x < W(1); x++) {
            double accum = 0;
            for (size_t y = 0; y < maxK; y++)
                accum += d0 [x % W(0)] * d1 [x + (y % H(1)) * W(1)];
            dOut[x] = accum;
        }
    }

    template <typename D0, typename D1, typename X>
    void matvec(const D0* d0, const D1* d1, X* dOut) const
    {
        const size_t maxK = max<size_t>(W(0), W(1));
        for (size_t x = 0; x < H(0); x++) {
            double accum = 0;
            for (size_t y = 0; y < maxK; y++)
                accum += d0 [(y % W(0)) + x * W(0)] * d1 [y % W(1)];
            dOut[x] = accum;
        }
    }

    template <typename D0, typename D1, typename X>
    void matmat(const D0* d0, const D1* d1, X* dOut) const
    {
        const size_t maxK = max<size_t>(W(0), H(1));
        for (size_t x = 0; x < W(1); x++)
        for (size_t y = 0; y < H(0); y++) {
            double accum = 0;
            for (size_t z = 0; z < maxK; z++)
                accum += d0 [(z % W(0)) + y * W(0)]
                       * d1 [x + (z % H(1)) * W(1)];
            dOut[x + y * W(1)] = accum;
        }
    }

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpMatmul(void);
};

}; // namespace chai_internal

#endif
