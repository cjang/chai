// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeStatements.hpp"
#include "CodeValues.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declaring variables

string declare(const IVariable& lhs,
               const Indent& indent)
{
    stringstream ss;

    ss << indent
       << lhs.declaredName()
       << ";"
       << endl;

    return ss.str();
}

string declare(const IVariable& lhs,
               const IValue& rhs,
               const Indent& indent)
{
    stringstream ss;

    ss << indent
       << lhs.declaredName()
       << " = "
       << rhs.name()
       << ";"
       << endl;

    return ss.str();
}

string declare(const IVariable& lhs,
               const size_t rhs,
               const Indent& indent)
{
    return declare(lhs, ConstantValue(rhs), indent);
}

string declare(const vector< PrivateVar >& lhs,
               const Indent& indent)
{
    stringstream ss;

    for (vector< PrivateVar >::const_iterator
         it = lhs.begin(); it != lhs.end(); it++)
    {
        ss << indent
           << (*it).declaredName()
           << ";"
           << endl;
    }

    return ss.str();
}

string declare(const vector< PrivateVar >& lhs,
               const IValue& rhs,
               const Indent& indent)
{
    stringstream ss;

    for (vector< PrivateVar >::const_iterator
         it = lhs.begin(); it != lhs.end(); it++)
    {
        ss << indent
           << (*it).declaredName()
           << " = "
           << rhs.name()
           << ";"
           << endl;
    }

    return ss.str();
}

string declare(const vector< PrivateVar >& lhs,
               const size_t rhs,
               const Indent& indent)
{
    return declare(lhs, ConstantValue(rhs), indent);
}

string declare(const vector< PrivateVar >& lhs,
               const IValue& rhs,
               const IValue& step,
               const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
    {
        ss << indent << lhs[i].declaredName() << " = ";

        if (0 == i)
            ss << rhs.name();
        else
            ss << (lhs[i-1] + step).name();

        ss << ";" << endl;
    }

    return ss.str();
}

string declare(const vector< PrivateVar >& lhs,
               const IValue& rhs,
               const size_t step,
               const Indent& indent)
{
    return declare(lhs, rhs, ConstantValue(step), indent);
}

string declare(const vector< vector< PrivateVar > >& lhs,
               const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
        ss << declare(lhs[i], indent);

    return ss.str();
}

string declare(const vector< vector< PrivateVar > >& lhs,
               const IValue& rhs,
               const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
        ss << declare(lhs[i], rhs, indent);

    return ss.str();
}

string declare(const vector< vector< PrivateVar > >& lhs,
               const size_t rhs,
               const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
        ss << declare(lhs[i], rhs, indent);

    return ss.str();
}

////////////////////////////////////////
// assigning variables

string assign(const IVariable& lhs,
              const IValue& rhs,
              const Indent& indent)
{
    stringstream ss;

    ss << indent
       << lhs.name()
       << " = "
       << rhs.name()
       << ";"
       << endl;

    return ss.str();
}

string assign(const IVariable& lhs,
              const size_t rhs,
              const Indent& indent)
{
    return assign(lhs, ConstantValue(rhs), indent);
}

string assign(const vector< PrivateVar >& lhs,
              const IValue& rhs,
              const Indent& indent)
{
    stringstream ss;

    for (vector< PrivateVar >::const_iterator
         it = lhs.begin(); it != lhs.end(); it++)
    {
        ss << indent
           << (*it).name()
           << " = "
           << rhs.name()
           << ";"
           << endl;
    }

    return ss.str();
}

string assign(const vector< PrivateVar >& lhs,
              const size_t rhs,
              const Indent& indent)
{
    return assign(lhs, ConstantValue(rhs), indent);
}

string assign(const vector< PrivateVar >& lhs,
              const IValue& rhs,
              const IValue& step,
              const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
    {
        ss << indent << lhs[i].name() << " = ";

        if (0 == i)
            ss << rhs.name();
        else
            ss << (lhs[i-1] + step).name();

        ss << ";" << endl;
    }

    return ss.str();
}

string assign(const vector< PrivateVar >& lhs,
              const IValue& rhs,
              const size_t step,
              const Indent& indent)
{
    return assign(lhs, rhs, ConstantValue(step), indent);
}

string assign(const vector< vector< PrivateVar > >& lhs,
              const IValue& rhs,
              const Indent& indent)
{
    stringstream ss;

    for (size_t i = 0; i < lhs.size(); i++)
        ss << assign(lhs[i], rhs, indent);

    return ss.str();
}

string assign(const vector< vector< PrivateVar > >& lhs,
              const size_t rhs,
              const Indent& indent)
{
    return assign(lhs, ConstantValue(rhs), indent);
}

////////////////////////////////////////
// incrementing variables

string increment(const IVariable& lhs,
                 const IValue& rhs,
                 const Indent& indent)
{
    stringstream ss;

    ss << indent
       << lhs.name()
       << " += "
       << rhs.name()
       << ";"
       << endl;

    return ss.str();
}

string increment(const IVariable& lhs,
                 const size_t rhs,
                 const Indent& indent)
{
    return increment(lhs, ConstantValue(rhs), indent);
}

string increment(const vector< PrivateVar >& lhs,
                 const IValue& rhs,
                 const Indent& indent)
{
    stringstream ss;

    for (vector< PrivateVar >::const_iterator
         it = lhs.begin(); it != lhs.end(); it++)
    {
        ss << indent
           << (*it).name()
           << " += "
           << rhs.name()
           << ";"
           << endl;
    }

    return ss.str();
}

string increment(const vector< PrivateVar >& lhs,
                 const size_t rhs,
                 const Indent& indent)
{
    return increment(lhs, ConstantValue(rhs), indent);
}

}; // namespace chai_internal
