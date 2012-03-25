// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ClientTrace.hpp"
#include "MemalignSTL.hpp"
#include "MemManager.hpp"

using namespace std;

namespace chai_internal {

ClientTrace::ClientTrace(void)
    : _refs(),
      _scheduleCheckpoint(0),
      _stickyDeviceCode()
{
    _stickyDeviceCode.push_back(-1); // no device yet
}

size_t ClientTrace::getScheduleCheckpoint(void)
{
    return _scheduleCheckpoint;
}

void ClientTrace::setScheduleCheckpoint(void)
{
    _scheduleCheckpoint = _lhsVariable.size();
}

void ClientTrace::pushHashCode(const uint64_t hashCode)
{
    _hashCodeHistory.push_back(hashCode);
}

const vector< uint64_t >& ClientTrace::hashCodeHistory(void) const
{
    return _hashCodeHistory;
}

size_t ClientTrace::stickyDevice(void) const
{
    return _stickyDeviceCode.back();
}

bool ClientTrace::stickyDevice(const size_t deviceCode)
{
    if (-1 == _stickyDeviceCode.back())
    {
        // not stuck to a device, so can stick to specified device
        _stickyDeviceCode.push_back(deviceCode);
        return true;
    }
    else
    {
        // stuck to a device already
        return deviceCode == _stickyDeviceCode.back();
    }
}

void ClientTrace::unstickyDevice(void)
{
    // unstick from device by adding to history
    _stickyDeviceCode.push_back(-1);
}

bool ClientTrace::stickyMovement(void) const
{
    // return true if:
    // 1. trace currently stuck to an OpenCL compute device
    // 2. trace was last stuck to another (different) OpenCL compute device
    // this means array memory needs to swizzle from the last compute device
    // to the cpu host and then transfer over to the new compute device

    size_t lastStuck = -1, nextLastStuck = -1;

    for (vector< size_t >::const_iterator
         it = _stickyDeviceCode.begin();
         it != _stickyDeviceCode.end();
         it++)
    {
        const size_t deviceCode = *it;

        if (-1 != deviceCode)
        {
            nextLastStuck = lastStuck;
            lastStuck = deviceCode;
        }
    }

    return -1 != lastStuck &&
           -1 != nextLastStuck &&
           MemManager::GPU_OPENCL <= lastStuck &&
           MemManager::GPU_OPENCL <= nextLastStuck &&
           lastStuck != nextLastStuck;
}

void ClientTrace::clear(void)
{
    _constructors.clear();
    _destructors.clear();
    _liveVariables.clear();

    _lhsVariable.clear();
    _lhsVersion.clear();
    _rhsBytecode.clear();

    _refs.clear();
    _frontMem.clear();

    _variableNuts.clear();

    _scheduleCheckpoint = 0;

    _hashCodeHistory.clear();
    _stickyDeviceCode.clear();
    _stickyDeviceCode.push_back(-1);
}

bool ClientTrace::final(void) const
{
    // all variables have passed out of scope
    return _constructors.size() == _destructors.size();
}

void ClientTrace::constructor(const uint32_t variable,
                              SingleNut* variableNut)
{
    _constructors.insert(variable);
    _liveVariables.insert(variable);

    _lhsVariable.push_back(variable);
    _lhsVersion.push_back(0);
    _rhsBytecode.push_back(Stak<BC>());

    _refs.checkout(variableNut);
    _variableNuts[variable] = variableNut;
}

void ClientTrace::destructor(const uint32_t variable)
{
    _destructors.insert(variable);
    _liveVariables.erase(variable);

    _lhsVariable.push_back(variable);
    _lhsVersion.push_back(-1);
    _rhsBytecode.push_back(Stak<BC>());
}

Stak<BC>& ClientTrace::assignment(const uint32_t variable,
                                  const uint32_t version)
{
    _lhsVariable.push_back(variable);
    _lhsVersion.push_back(version);
    _rhsBytecode.push_back(Stak<BC>());

    return _rhsBytecode.back();
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const size_t precision)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, precision);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const size_t precision,
                                const size_t slots)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, precision, slots);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                uint32_t* defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                int32_t* defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                float* defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                double* defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< uint32_t* >& defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< int32_t* >& defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< float* >& defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

FrontMem* ClientTrace::memalloc(const uint32_t variable,
                                const size_t W,
                                const size_t H,
                                const vector< double* >& defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(variable, W, H, defPtr);
    _refs.checkout(m);
    return m;
}

size_t ClientTrace::frontMem(FrontMem* m)
{
    // This index is the absolute statement index, not relative.
    // It is stored in the bytecode trace itself as a pointer value.
    const size_t idx = _lhsVariable.size();
    _frontMem[ idx ] = m;
    return idx;
}

}; // namespace chai_internal
