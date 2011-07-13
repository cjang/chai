// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "HashBC.hpp"
#include "SingleTrace.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

uint64_t SingleTrace::computeHash(ClientTrace& origin)
{
    vector< uint64_t > hashStmt;

    for (size_t i = 0; i < _lhsVariable.size(); i++)
    {
        HashBC v;

        v.push(_lhsVariable[i]);
        v.push(_lhsVersion[i]);

        _rhsBytecode[i].ascend(v);

        hashStmt.push_back(v.code());
    }

    return TEA::hash(&hashStmt[0], hashStmt.size());
}

SingleTrace::SingleTrace(ClientTrace& origin)
    : _liveVariables(origin._liveVariables),
      _variableNuts(origin._variableNuts)
{
    // copy trace statements from last checkpoint
    for (size_t i = origin.getScheduleCheckpoint();
                i < origin._lhsVariable.size();
                i++)
    {
        _lhsVariable.push_back(origin._lhsVariable[i]);
        _lhsVersion.push_back(origin._lhsVersion[i]);
        _rhsBytecode.push_back(origin._rhsBytecode[i]);
    }

    // limit boxed array memory to statements from last checkpoint
    for (map< size_t, FrontMem* >::const_iterator
         it = origin._frontMem.begin();
         it != origin._frontMem.end();
         it++)
    {
        const size_t stmtIndex = (*it).first;
        if ( stmtIndex >= origin.getScheduleCheckpoint() &&
             stmtIndex < origin._lhsVariable.size() )
        {
            _frontMem[ stmtIndex ] = origin._frontMem[ stmtIndex ];
            _frontVar[ stmtIndex ] = origin._lhsVariable[ stmtIndex ];
        }
    }

    // move checkpoint
    origin.setScheduleCheckpoint();

    // compute hash code
    _hashCode = computeHash(origin);
}

uint64_t SingleTrace::hashCode(void) const
{
    return _hashCode;
}

}; // namespace chai_internal
