// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "CodeControlFlow.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// special things like barriers and loops

// LocalBarrier
LocalBarrier::LocalBarrier(Indent& indent)
    : IPrintable(indent) { }

ostream& LocalBarrier::print(ostream& os) const
{
    return endline(os << _indent << "barrier(CLK_LOCAL_MEM_FENCE)");
}

// ForLoop
ForLoop::ForLoop(const IVariable& index,
                 const IValue& limit,
                 const size_t step,
                 Indent& indent)
    : IPrintable(indent),
      _index(index),
      _limitNum(),
      _limit(limit),
      _increment(step) { }

ForLoop::ForLoop(const IVariable& index,
                 const size_t limit,
                 const size_t step,
                 Indent& indent)
    : IPrintable(indent),
      _index(index),
      _limitNum(limit),
      _limit(_limitNum),
      _increment(step) { }

ostream& ForLoop::print(ostream& os) const
{
    os << _indent
       << "for (int " << _index.name() << " = 0; "
       << _index.name() << " < " << _limit.name() << "; "
       << _index.name();

    if (1 == _increment)
        os << "++";
    else
        os << " += " << _increment;

    os << ")" << endl
       << _indent
       << "{" << endl;

    _indent.more();

    return os;
}

// EndBlock
EndBlock::EndBlock(Indent& indent)
    : IPrintable(indent) { }

ostream& EndBlock::print(ostream& os) const
{
    return os << _indent.less()
              << "}"
              << endl;
}

}; // namespace chai_internal
