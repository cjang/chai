// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_UTIL_FUNS_HPP_
#define _CHAI_UTIL_FUNS_HPP_

namespace chai_internal {

template <typename X, typename Y, typename Z>
X min(const Y a, const Z b)
{
    return (a > b) ? b : a;
}

template <typename X, typename Y, typename Z>
X max(const Y a, const Z b)
{
    return (a > b) ? a : b;
}

}; // namespace chai_internal

#endif
