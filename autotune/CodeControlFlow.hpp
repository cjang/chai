// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_CONTROL_FLOW_HPP_
#define _CHAI_CODE_CONTROL_FLOW_HPP_

#include <ostream>
#include <string>

#include "CodeFormat.hpp"
#include "CodeInterfaces.hpp"
#include "CodeValues.hpp"

namespace chai_internal {

////////////////////////////////////////
// special things like barriers and loops

class LocalBarrier : public IPrintable
{
public:
    LocalBarrier(Indent& indent = Indent::defaultObj());

    std::ostream& print(std::ostream& os) const;
};

class ForLoop : public IPrintable
{
    const IVariable&    _index;
    const ConstantValue _limitNum;
    const IValue&       _limit;
    const size_t        _increment;

public:
    ForLoop(const IVariable& index,
            const IValue& limit,
            const size_t step,
            Indent& indent = Indent::defaultObj());

    ForLoop(const IVariable& index,
            const size_t limit,
            const size_t step,
            Indent& indent = Indent::defaultObj());

    std::ostream& print(std::ostream& os) const;
};

class EndBlock : public IPrintable
{
public:
    EndBlock(Indent& indent = Indent::defaultObj());

    std::ostream& print(std::ostream& os) const;
};

}; // namespace chai_internal

#endif
