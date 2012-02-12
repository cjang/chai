// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_QUALIFIER_HPP_
#define _CHAI_QUALIFIER_HPP_

#include <ostream>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// qualifier for variables and functions

class Qualifier
{
    const std::string _str;

protected:
    Qualifier(const std::string&);

public:
    virtual ~Qualifier(void);

    std::string str(void) const;
};

std::ostream& operator<< (std::ostream&, const Qualifier&);

////////////////////////////////////////
// address space qualifier

struct AddressSpace : public Qualifier
{
    AddressSpace(const std::string&);
};

static const AddressSpace GLOBAL("__global ");
static const AddressSpace LOCAL("__local ");
static const AddressSpace CONSTANT("__constant ");
static const AddressSpace PRIVATE("__private ");
static const AddressSpace DEFAULT("");

////////////////////////////////////////
// image access qualifier

struct ImageAccess : public Qualifier
{
    ImageAccess(const std::string&);
};

static const ImageAccess READONLY("__read_only ");
static const ImageAccess WRITEONLY("__write_only ");

////////////////////////////////////////
// function attribute qualifier

struct FunctionAttribute : public Qualifier
{
    FunctionAttribute(const std::string&);
};

static const FunctionAttribute KERNEL("__kernel ");

}; // namespace chai_internal

#endif
