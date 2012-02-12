// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_FUNCTION_HPP_
#define _CHAI_CODE_FUNCTION_HPP_

#include <ostream>
#include <string>
#include <vector>

#include "CodeFormat.hpp"
#include "CodeInterfaces.hpp"

namespace chai_internal {

////////////////////////////////////////
// function declaration

class FunctionDeclaration : public IPrintable
{
    const std::string               _functionName;
    std::vector< const IVariable* > _arguments;

public:
    FunctionDeclaration(const std::string functionName,
                        Indent& indent = Indent::defaultObj());

    void argument(const IVariable& variable);

    std::ostream& print(std::ostream& os) const;
};

}; // namespace chai_internal

#endif
