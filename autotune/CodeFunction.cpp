// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "CodeFunction.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// function declaration

FunctionDeclaration::FunctionDeclaration(const string functionName,
                                         Indent& indent)
    : IPrintable(indent),
      _functionName(functionName) { }

void FunctionDeclaration::argument(const IVariable& variable)
{
    _arguments.push_back(&variable);
}

ostream& FunctionDeclaration::print(ostream& os) const
{
    os << _indent
       << "__kernel void "
       << _functionName
       << "(";

    _indent.more();

    // function arguments
    for (size_t i = 0; i < _arguments.size(); i++)
    {
        os << endl
           << _indent
           << _arguments[i]->declaredName();

        if (i < _arguments.size()-1)
            os << ",";
    }

    _indent.less();

    os << ")"
       << endl
       << _indent
       << "{" << endl;

    _indent.more();

    return os;
}

}; // namespace chai_internal
