// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EDIT_STAK_HPP_
#define _CHAI_EDIT_STAK_HPP_

#include <map>
#include <set>
#include <utility>

#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "chai/Visit.hpp"

namespace chai_internal {

class EditStak : public Visit<BC>
{
    Stak<BC> _newBytecode;

    bool     _isEdited;

    // original variable -> replacement variable
    std::map< uint32_t, uint32_t > _replaceVar;

    // original variable -> replacement bytecode
    std::map< uint32_t, Stak<BC> > _replaceVarBC;

    // for JIT hash codes of RNG statements
    std::map< uint32_t, std::set< size_t > > _nullifyOpArg;
    uint32_t _currentOpCode;
    size_t   _currentArgIndex;

public:
    EditStak(void);

    void clear(void);

    void replaceVariable(const uint32_t fromVariable,
                         const uint32_t toVariable);

    void replaceVariableBC(const uint32_t fromVariable,
                           const Stak<BC>& toBC);

    void nullifyOpArg(const uint32_t opCode,
                      const size_t argIndex);

    bool changed(void) const;

    const Stak<BC>& getBytecode(void) const;

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
