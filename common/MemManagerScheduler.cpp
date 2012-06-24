// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <string.h>

#include "Logger.hpp"
#include "MemalignSTL.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

bool MemManager::memalloc(VectorTrace& vt)
{
    // iterate over statements that have array memory boxes
    for (map< size_t, vector< FrontMem* > >::const_iterator
         it = vt.frontMem().begin();
         it != vt.frontMem().end();
         it++)
    {
        const FrontMem* proto = (*it).second.front();

        // Special case of wrapped void* pointer for inline OpenCL
        if ( 0 == proto->W() && 0 == proto->H() )
            continue; // don't create any backing memory

        // The API allocates array memory boxes only.
        // If the first one is empty, then they all are.
        // Deferred allocation is all or nothing.
        if ( NULL != proto->ptrMem() )
            continue; // skip if memory already allocated

        map< void*, size_t > dataPtrMap;
        vector< size_t >     frontIndex;

        // A single vectorized statement is all make or all read boxes.
        // This is a side-effect of bytecode hashing and the API.
        // If one array memory box has a deferred pointer, then they all do.
        if (proto->dataPtr())
        {
            // must be make boxes

            // determine distinct data pointers for loop fusion
            for (vector< FrontMem* >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                void* p = (*jt)->dataPtr();

                if (0 == dataPtrMap.count(p))
                {
                    const size_t idx = dataPtrMap.size();
                    dataPtrMap[p] = idx;
                }

                frontIndex.push_back(dataPtrMap[p]);
            }
        }
        else
        {
            // must be read boxes

            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                frontIndex.push_back(i);
            }
        }

        const size_t numBackingSlots = dataPtrMap.empty()
                                           ? vt.numTraces()     // read boxes
                                           : dataPtrMap.size(); // make boxes

        // size of memory for each front memory box
        const size_t frontWidth = PrecType::padWidth(proto->precision(),
                                                     proto->W());
        const size_t frontSize
            = proto->H() * frontWidth * PrecType::sizeOf(proto->precision());

        BackMem* backMem = NULL;

        // continuation check for pre-existing allocation
        const uint32_t traceVar = proto->variable();
        if (-1 != traceVar && vt.liveVariables().count(traceVar))
        {
            for (vector< uint64_t >::const_iterator
                 jt = vt.hashCodeHistory().begin();
                 jt != vt.hashCodeHistory().end();
                 jt++)
            {
                const uint64_t hashCode = *jt;

                // not locking as there is no mutation, hopefully this is safe
                if (_liveArray.count(hashCode) &&
                    _liveArray[hashCode].count(traceVar))
                {
                    ArrayBuf* arrayBuf = _liveArray[hashCode][traceVar];

                    // get the back memory object
                    if (_arrayBack.count(arrayBuf))
                    {
                        backMem = _arrayBack[arrayBuf];
                    }
                }
            }
        }

        void* b = NULL;

        // allocate backing memory
        if (NULL == backMem)
        {
            if (0 == posix_memalign(&b,
                                    MEMORY_ALIGNMENT,
                                    frontSize * numBackingSlots))
            {
                // allows summing oversized buffer in read_scalar()
                // initialize memory to zero
                memset(b, 0, frontSize * numBackingSlots);

                switch (proto->precision())
                {
                    case (PrecType::UInt32) :
                        backMem = new BackMem(frontWidth,
                                              proto->H(),
                                              numBackingSlots,
                                              vt.numTraces(), // front count
                                              static_cast<uint32_t*>(b),
                                              this);
                        break;

                    case (PrecType::Int32) :
                        backMem = new BackMem(frontWidth,
                                              proto->H(),
                                              numBackingSlots,
                                              vt.numTraces(), // front count
                                              static_cast<int32_t*>(b),
                                              this);
                        break;

                    case (PrecType::Float) :
                        backMem = new BackMem(frontWidth,
                                              proto->H(),
                                              numBackingSlots,
                                              vt.numTraces(), // front count
                                              static_cast<float*>(b),
                                              this);
                        break;

                    case (PrecType::Double) :
                        backMem = new BackMem(frontWidth,
                                              proto->H(),
                                              numBackingSlots,
                                              vt.numTraces(), // front count
                                              static_cast<double*>(b),
                                              this);
                        break;
                }
            }
            else
            {
                // posix_memalign failed
                return false;
            }
        }

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << "backMem " << backMem << " for trace variable " << traceVar;
        LOGGER(ss.str())
#endif

        // iterate over statement array memory boxes
        for (size_t i = 0; i < (*it).second.size(); i++)
        {
            (*it).second[i]->setBackMem(
                static_cast< char* >(b) + frontIndex[i] * frontSize,
                backMem);
        }

        // check if re-used old backing memory or allocated new object
        if (NULL != b)
        {
            // front memory owns the backing memory
            backMem->release();
        }

        // copy deferred data
        if (proto->dataPtr())
        {
            for (map< void*, size_t >::const_iterator
                 jt = dataPtrMap.begin();
                 jt != dataPtrMap.end();
                 jt++)
            {
                // Padding makes the front memory boxes larger than the
                // deferred data. Row-major memory layout is also an issue.
                if (frontWidth > proto->W())
                {
                    const size_t dataRowSize
                        = proto->W() * PrecType::sizeOf(proto->precision());

                    const size_t padRowSize
                        = frontWidth * PrecType::sizeOf(proto->precision());

                    for (size_t j = 0; j < proto->H(); j++)
                    {
                        memcpy(static_cast< char* >(b)
                                   + (*jt).second * frontSize
                                   + padRowSize * j,
                               static_cast< char* >((*jt).first)
                                   + dataRowSize * j,
                               dataRowSize);

                        memset(static_cast< char* >(b)
                                   + (*jt).second * frontSize
                                   + padRowSize * j
                                   + dataRowSize,
                               0,
                               padRowSize - dataRowSize);
                    }
                }
                else
                {
                    // no padding, simple memory copy
                    memcpy(static_cast< char* >(b)
                               + (*jt).second * frontSize,
                           (*jt).first,
                           frontSize);
                }
            }
        }

        const size_t stmtIndex = (*it).first;

        // set backing memory in vector trace
        vt.backMem()[stmtIndex] = backMem;

        // remember readout and makedata memalloc layout
        if (-1 != proto->variable())
        {
            vt.memallocFront(
                   proto->variable(),
                   frontSize,
                   frontIndex );
        }
    }

    // success
    return true;
}

void MemManager::memfree(VectorTrace& vt, const bool freeMem)
{
    const uint64_t hashCode = vt.hashCode();

    // if device failed, then free immediately
    if (freeMem)
    {
        // iterate over vector trace statements with allocated memory
        for (map< size_t, vector< FrontMem* > >::const_iterator
             it = vt.frontMem().begin();
             it != vt.frontMem().end();
             it++)
        {
            // clear memory in every trace, release reference counts
            // this will delete backing memory objects
            for (vector< FrontMem* >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                (*jt)->clearBackMem();
            }

            const size_t stmtIndex = (*it).first;

            // clear backing parent mem in vector trace (not deleting it!)
            vt.backMem().erase(stmtIndex);
        }

        pthread_mutex_lock(&_memMutex);

        // clean up compute device memory for live trace variables
        if (_liveArray.count(hashCode))
        {
            for (map< uint32_t, ArrayBuf* >::const_iterator
                 it = _liveArray[hashCode].begin();
                 it != _liveArray[hashCode].end();
                 it++)
            {
                ArrayBuf* arrayBuf = (*it).second;

                if (_arrayBack.count(arrayBuf))
                {
                    BackMem* backMem = _arrayBack[arrayBuf];

                    _backArray.erase(backMem);
                    _arrayBack.erase(arrayBuf);
                    _arrayCount.erase(arrayBuf);
                }

                delete arrayBuf;
            }

            _liveArray.erase(hashCode);
        }

        pthread_mutex_unlock(&_memMutex);
    }

    // clean up compute device memory for dead trace variables
    if (_deadArray.count(hashCode))
    {
        for (map< uint32_t, ArrayBuf* >::const_iterator
             it = _deadArray[hashCode].begin();
             it != _deadArray[hashCode].end();
             it++)
        {
            delete (*it).second;
        }

        _deadArray.erase(hashCode);
    }

    // split variables persist for duration of scheduled kernels only
    if (_splitArray.count(hashCode))
    {
        for (map< const AstVariable*, ArrayBuf* >::const_iterator
             it = _splitArray[hashCode].begin();
             it != _splitArray[hashCode].end();
             it++)
        {
            delete (*it).second;
        }

        _splitArray.erase(hashCode);
    }
}

}; // namespace chai_internal
