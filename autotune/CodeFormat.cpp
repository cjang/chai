// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "CodeFormat.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// convert ordinal digits to hex string
// for vector type components

const char* hex(const size_t n)
{
    static const char *lut[] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "A", "B", "C", "D", "E", "F"
    };

    return (n < 16)
               ? lut[n]
               : "X";   // deliberately invalid value
}

////////////////////////////////////////
// indentation of source code text

Indent::Indent(void)
    : _tabs(0) { }

Indent::Indent(const size_t num)
    : _tabs(num) { }

Indent::Indent(const Indent& other)
    : _tabs(other._tabs) { }

Indent& Indent::more(void)
{
    _tabs++;
    return *this;
}

Indent& Indent::less(void)
{
    _tabs--;
    return *this;
}

Indent& Indent::set(const Indent& other)
{
    _tabs = other._tabs;
    return *this;
}

string Indent::str(void) const
{
    string s;
    for (size_t i = 0; i < _tabs; i++)
        s.append("    ");
    return s;
}

Indent& Indent::defaultObj(void)
{
    static Indent defaultObj;
    return defaultObj;
}

ostream& operator<< (ostream& os,
                     const Indent& tabs)
{
    return os << tabs.str();
}

ostream& endline(ostream& os)
{
    return os << ";" << endl;
}

}; // namespace chai_internal
