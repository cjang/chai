// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_MATMUL_BASE_HPP_
#define _CHAI_INTERP_MATMUL_BASE_HPP_

#include "BaseInterp.hpp"
#include "UtilFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// simple and general matmul base class

class InterpMatmulBase : public BaseInterp
{
protected:
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
                accum += d0 [y % W(0)] * d1 [x + (y % H(1)) * W(1)];
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

    template <typename D0, typename D1, typename X>
    void addgen(const D0* d0,
                const D1* d1,
                const size_t dW,
                const size_t dH,
                X* dOut) const
    {
        const double alpha = _argScalar[0];
        const double beta = _argScalar[1];
        for (size_t x = 0; x < W(2); x++)
        for (size_t y = 0; y < H(2); y++)
            dOut[x + y * W(2)] = alpha * d0[(x % dW) + (y % dH) * dW]
                               + beta * d1[x + y * W(2)];
    }

    void doMatmul(std::stack< std::vector< FrontMem* > >&);

public:
    InterpMatmulBase(const size_t inCount,
                     const size_t outCount);
};

}; // namespace chai_internal

#endif
