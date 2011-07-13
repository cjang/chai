// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "EditStak.hpp"

using namespace std;

namespace chai_internal {

EditStak::EditStak(void)
    : _isEdited(false) { }

void EditStak::clear(void)
{
    _newBytecode.clear();
}

void EditStak::replaceVariable(const uint32_t fromVariable,
                               const uint32_t toVariable)
{
    _replaceVar[ fromVariable ] = toVariable;
}

bool EditStak::changed(void) const
{
    return _isEdited;
}

const Stak<BC>& EditStak::getBytecode(void) const
{
    return _newBytecode;
}

void EditStak::visit(const uint32_t variable, const uint32_t version)
{
    if (0 == _replaceVar.count(variable))
    {
        // no change
        _newBytecode.push( BC(variable, version) );
    }
    else
    {
        // change the variable
        _newBytecode.push( BC(_replaceVar[variable], version) );
        _isEdited = true;
    }
}

void EditStak::visit(const uint32_t opCode)
{
    _newBytecode.push(opCode);
}

void EditStak::visit(const double scalar)
{
    _newBytecode.push(scalar);
}

void EditStak::visit(void* ptr)
{
    _newBytecode.push(ptr);
}

}; // namespace chai_internal
