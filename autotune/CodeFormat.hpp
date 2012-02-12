// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_FORMAT_HPP_
#define _CHAI_CODE_FORMAT_HPP_

#include <ostream>
#include <sstream>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// functions as string

template <typename T>
std::string as_string(const T& value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template <typename T1>
std::string func_string(const char *func,
                        const T1& arg1)
{
    std::string s = func;
    return s.append("(").append(as_string<T1>(arg1)).append(")");
}

template <typename T1, typename T2>
std::string func_string(const char *func,
                        const T1& arg1,
                        const T2& arg2)
{
    std::string s = func;
    return s.append("(")
            .append(as_string<T1>(arg1))
            .append(", ")
            .append(as_string<T2>(arg2))
            .append(")");
}

template <typename T1, typename T2, typename T3>
std::string func_string(const char *func,
                        const T1& arg1,
                        const T2& arg2,
                        const T3& arg3)
{
    std::string s = func;
    return s.append("(")
            .append(as_string<T1>(arg1))
            .append(", ")
            .append(as_string<T2>(arg2))
            .append(", ")
            .append(as_string<T3>(arg3))
            .append(")");
}

////////////////////////////////////////
// convert ordinal digits to hex string
// for vector type components

const char* hex(const size_t n);

////////////////////////////////////////
// indentation of source code text

class Indent
{
    size_t _tabs;

public:
    Indent(void);
    Indent(const size_t num);
    Indent(const Indent& other);

    Indent& more(void);
    Indent& less(void);
    Indent& set(const Indent& other);

    std::string str(void) const;
    static Indent& defaultObj(void);
};

std::ostream& operator<< (std::ostream& os, const Indent& tabs);

std::ostream& endline(std::ostream& os);

}; // namespace chai_internal

#endif
