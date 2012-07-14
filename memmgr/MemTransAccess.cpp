// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemTrans.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// accessors

int MemTrans::deviceNum(void) const
{
    return _deviceNum;
}

OCLdevice* MemTrans::computeDevice(void) const
{
    return _oclDevice;
}

ArrayBuf* MemTrans::arrayBuf(const uint32_t traceVar,
                             VectorTrace& vt)
{
    // start with most recent hash code and work backwards in history
    for (vector< uint64_t >::const_reverse_iterator
         it = vt.hashCodeHistory().rbegin();
         it != vt.hashCodeHistory().rend();
         it++)
    {
        const uint64_t hashCode = *it;

        // not locking as there is no mutation, hopefully this is safe
        if (_liveArray.count(hashCode) &&
            _liveArray[hashCode].count(traceVar))
        {
            return _liveArray[hashCode][traceVar];
        }
        else if (_deadArray.count(hashCode) &&
                 _deadArray[hashCode].count(traceVar))
        {
            return _deadArray[hashCode][traceVar];
        }
    }

    // nothing found
    return NULL;
}

ArrayBuf* MemTrans::arrayBuf(const AstVariable* splitVar,
                             VectorTrace& vt)
{
    // start with most recent hash code and work backwards in history
    for (vector< uint64_t >::const_reverse_iterator
         it = vt.hashCodeHistory().rbegin();
         it != vt.hashCodeHistory().rend();
         it++)
    {
        const uint64_t hashCode = *it;

        if (_splitArray.count(hashCode) &&
            _splitArray[hashCode].count(splitVar))
        {
            return _splitArray[hashCode][splitVar];
        }
    }

    return NULL;
}

}; // namespace chai_internal
