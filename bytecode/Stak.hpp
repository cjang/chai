// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STAK_HPP_
#define _CHAI_STAK_HPP_

#include <vector>

#include "chai/Visit.hpp"

namespace chai_internal {

////////////////////////////////////////
// generic stack

template <typename X>
class Stak
{
    std::vector<X> _stak;

public:
    Stak(void)
        : _stak() { }

    Stak(const Stak& other)
        : _stak(other._stak) { }

    Stak& operator= (const Stak& other)
    {
        _stak = other._stak;
        return *this;
    }

    size_t size(void) const { return _stak.size(); }

    void clear(void) { _stak.clear(); }

    void push(const X& obj) { _stak.push_back(obj); }

    void push(const Stak& other)
    {
        for (size_t i = 0; i < other._stak.size(); i++)
            _stak.push_back(other._stak[i]);
    }

    void top(Visit<X>& v) const
    {
        X obj = _stak.back();
        obj.accept(v);
    }

    void pop(Visit<X>& v)
    {
        X obj = _stak.back();
        _stak.pop_back();
        obj.accept(v);
    }

    void descend(Visit<X>& v) const
    {
        for (int i = _stak.size()-1; i >= 0; i--)
            _stak[i].accept(v);
    }

    void ascend(Visit<X>& v) const
    {
        for (size_t i = 0; i < _stak.size(); i++)
            _stak[i].accept(v);
    }
};

}; // namespace chai_internal

#endif
