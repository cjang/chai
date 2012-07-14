// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemTrans.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// callbacks from back memory

// data movement from compute device back to CPU
void MemTrans::swizzle(BackMem* backMem)
{
    pthread_mutex_lock(&_memMutex);

    // is this object still managed?
    if ( _backArray.count(backMem) )
    {
        // OpenCL enqueue memory read and wait here
        _backArray[backMem]->flushFromDevice();
    }

    pthread_mutex_unlock(&_memMutex);
}

// called from the BackMem destructor
void MemTrans::untrackMem(BackMem* backMem)
{
    pthread_mutex_lock(&_memMutex);

    if (_backArray.count(backMem))
    {
        ArrayBuf* arrayBuf = _backArray[backMem];

        if (0 == --_arrayCount[arrayBuf])
        {
            _backArray.erase(backMem);
            _arrayBack.erase(arrayBuf);
            _arrayCount.erase(arrayBuf);

            delete arrayBuf;

            map< uint64_t, uint32_t > eraseKeys;

            for (map< uint64_t, map< uint32_t, ArrayBuf* > >::const_iterator
                 it = _liveArray.begin(); it != _liveArray.end(); it++)
            {
                for (map< uint32_t, ArrayBuf* >::const_iterator
                     jt = (*it).second.begin(); jt != (*it).second.end(); jt++)
                {
                    if ((*jt).second == arrayBuf)
                        eraseKeys[(*it).first] = (*jt).first;
                }
            }

            for (map< uint64_t, uint32_t >::const_iterator
                 it = eraseKeys.begin(); it != eraseKeys.end(); it++)
            {
                _liveArray[(*it).first].erase((*it).second);

                if (_liveArray[(*it).first].empty())
                    _liveArray.erase((*it).first);
            }
        }
    }

    pthread_mutex_unlock(&_memMutex);
}

}; // namespace chai_internal
