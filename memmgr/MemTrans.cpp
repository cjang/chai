// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemTrans.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// constructor, destructor

MemTrans::MemTrans(const int deviceNum,
                   OCLinit& o,
                   OCLdevice& d)
    : _deviceNum(deviceNum),
      _deviceIsCPU(o.devices().isCPU(deviceNum)),
      _oclInit(&o),
      _oclDevice(&d),
      _cache(new KernelCache(d))
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemTrans::~MemTrans(void)
{
    pthread_mutex_destroy(&_memMutex);

    delete _cache;
}

}; // namespace chai_internal
