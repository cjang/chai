// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_INTERFACES_HPP_
#define _CHAI_CODE_INTERFACES_HPP_

#include <ostream>
#include <string>

#include "CodeFormat.hpp"

namespace chai_internal {

////////////////////////////////////////
// interfaces

// values only appear on the right hand side
class IValue
{
public:
    virtual ~IValue(void);
    virtual std::string name(void) const = 0;
};

// variables appear on the left (declaration) and right (identifier) hand sides
class IVariable : public IValue
{
public:
    virtual ~IVariable(void);
    virtual std::string name(void) const = 0;
    virtual std::string declaredName(void) const = 0;
};

// print code text
class IPrintable
{
protected:
    Indent& _indent;

    IPrintable(Indent& indent);

public:
    virtual ~IPrintable(void);
    virtual std::ostream& print(std::ostream&) const = 0;
};

std::ostream& operator<< (std::ostream& os, const IPrintable& v);

}; // namespace chai_internal

#endif
