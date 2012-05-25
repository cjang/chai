// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "EditStak.hpp"

using namespace std;

namespace chai_internal {

EditStak::EditStak(void)
    : _isEdited(false),
      _currentOpCode(-1),
      _currentArgIndex(-1) { }

void EditStak::clear(void)
{
    _newBytecode.clear();
    _currentOpCode = -1;
    _currentArgIndex = -1;
}

void EditStak::replaceVariable(const uint32_t fromVariable,
                               const uint32_t toVariable)
{
    _replaceVar[ fromVariable ] = toVariable;
}

void EditStak::replaceVariableBC(const uint32_t fromVariable,
                                 const Stak<BC>& toBC)
{
    _replaceVarBC[ fromVariable ] = toBC;
}

void EditStak::nullifyOpArg(const uint32_t opCode,
                            const size_t argIndex)
{
    _nullifyOpArg[ opCode ].insert( argIndex );
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
        if (0 == _replaceVarBC.count(variable))
        {
            // no change
            _newBytecode.push( BC(variable, version) );
        }
        else
        {
            // change the variable, replace with bytecode
            _newBytecode.push( _replaceVarBC[variable] );
            _isEdited = true;
        }
    }
    else
    {
        // note: variable replacement overrides bytecode in case of both

        // change the variable, replace with another variable
        _newBytecode.push( BC(_replaceVar[variable], version) );
        _isEdited = true;
    }
}

void EditStak::visit(const uint32_t opCode)
{
    _newBytecode.push(opCode);

    _currentOpCode = opCode;
    _currentArgIndex = 0;
}

void EditStak::visit(const double scalar)
{
    if (_nullifyOpArg.count(_currentOpCode) &&
        _nullifyOpArg[_currentOpCode].count(_currentArgIndex))
    {
        _newBytecode.push(static_cast<double>(0));
    }
    else
    {
        _newBytecode.push(scalar);
    }

    if (-1 != _currentArgIndex)
        _currentArgIndex++;
}

void EditStak::visit(void* ptr)
{
    if (_nullifyOpArg.count(_currentOpCode) &&
        _nullifyOpArg[_currentOpCode].count(_currentArgIndex))
    {
        _newBytecode.push(static_cast<void*>(0));
    }
    else
    {
        _newBytecode.push(ptr);
    }

    if (-1 != _currentArgIndex)
        _currentArgIndex++;
}

}; // namespace chai_internal
