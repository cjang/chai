// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ClientTrace.hpp"
#include "MemalignSTL.hpp"

using namespace std;

namespace chai_internal {

ClientTrace::ClientTrace(void)
    : _refs(),
      _scheduleCheckpoint(0) { }

size_t ClientTrace::getScheduleCheckpoint(void)
{
    return _scheduleCheckpoint;
}

void ClientTrace::setScheduleCheckpoint(void)
{
    _scheduleCheckpoint = _lhsVariable.size();
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
                                float* defPtr)
{
    // memory manager sees this FrontMem when it allocates backing storage
    // note this allocation occurs before scheduling so it is associated
    // with the client application thread, not anything managed yet
    FrontMem* m = new FrontMem(W, H, defPtr);
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
    FrontMem* m = new FrontMem(W, H, defPtr);
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
