// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_RNG_HPP_
#define _CHAI_INTERP_RNG_HPP_

#include <math.h>
#include <stack>
#include <stdlib.h>

namespace chai_internal {

////////////////////////////////////////
// random number generator abstraction

class InterpRNG
{
    struct random_data _randbuf;
    static const size_t STATELEN = 256;
    char _statebuf[STATELEN];

    std::stack<double> _polar;

public:
    void seed(const unsigned int seed);

    template <typename X>
    X uniform(const size_t step)
    {
        int32_t randresult;

        for (size_t i = 0; i < step; i++)
            random_r(&_randbuf, &randresult);

        return randresult;
    }

    template <typename X>
    X uniform(const size_t step, const X minLimit, const X maxLimit)
    {
        int32_t randresult;

        for (size_t i = 0; i < step; i++)
            random_r(&_randbuf, &randresult);

        X x = randresult;
        x /= RAND_MAX;
        return x * (maxLimit - minLimit) + minLimit;
    }

    template <typename X>
    X normal(void)
    {
        if (_polar.empty()) {
            bool keepGoing = true;
            while (keepGoing) {
                const X u = uniform<X>(1, -1, 1);
                const X v = uniform<X>(1, -1, 1);
                const X s = u*u + v*v;
                if (0 < s && s < 1) {
                    if (sizeof(X) == sizeof(float)) {
                        const float c = sqrtf( (-2 * log(s)) / s );
                        _polar.push(u * c);
                        _polar.push(v * c);
                        keepGoing = false;
                    }
                    if (sizeof(X) == sizeof(double)) {
                        const double c = sqrt( (-2 * log(s)) / s );
                        _polar.push(u * c);
                        _polar.push(v * c);
                        keepGoing = false;
                    }
                }
            }
        }

        const X x = _polar.top();
        _polar.pop();
        return x;
    }
};

}; // namespace chai_internal

#endif
