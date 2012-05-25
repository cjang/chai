// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_GATHER_HPP_
#define _CHAI_INTERP_GATHER_HPP_

#include "BaseInterp.hpp"
#include "SimpleFuns.hpp"

namespace chai_internal {

////////////////////////////////////////
// gather1_floor, gather2_floor

class InterpGather : public BaseInterp
{
    const size_t _N;
    const _FLOOR_ _fun;

    template <typename D0, typename D1>
    void gather1_floor(const D0* d0, const D1* d1, D0* dOut) const
    {
        for (size_t x = 0; x < W(0); x++)
        for (size_t y = 0; y < H(0); y++) {
            const int xf = _fun( d1 [idx(1, x, y)] );
            const int xfM = (xf < 0) ? xf + W(0) : xf;
            const int xfR = xfM % W(0);
            dOut[x + y * W(0)] = d0 [xfR + y * W(0)];
        }
    }

    template <typename D0, typename D1, typename D2>
    void gather2_floor(const D0* d0, const D1* d1, const D2* d2, D0* dOut) const
    {
        for (size_t x = 0; x < W(0); x++)
        for (size_t y = 0; y < H(0); y++) {
            const int xf = _fun( d1 [idx(1, x, y)] );
            const int xfM = (xf < 0) ? xf + W(0) : xf;
            const int xfR = xfM % W(0);
            const int yf = _fun( d2 [idx(2, x, y)] );
            const int yfM = (yf < 0) ? yf + H(0) : yf;
            const int yfR = yfM % H(0);
            dOut[x + y * W(0)] = d0 [xfR + yfR * W(0)];
        }
    }

protected:
    void sub_eval(std::stack< std::vector< FrontMem* > >&);

public:
    InterpGather(const size_t N);
};

}; // namespace chai_internal

#endif
