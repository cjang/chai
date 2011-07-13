// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_PRINT_BC_HPP_
#define _CHAI_PRINT_BC_HPP_

#include <ostream>
#include <string>

#include "BC.hpp"

namespace chai_internal {

class PrintBC : public Visit<BC>
{
    std::ostream&     _os;
    const std::string _sep;
    void*             _ptrValue;

public:
    PrintBC(std::ostream& os, const std::string& sep);

    size_t ptrIndex(void) const;

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
