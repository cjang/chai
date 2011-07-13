// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "SingleNut.hpp"

using namespace std;

namespace chai_internal {

SingleNut::SingleNut(void)
    : RefObj(),
      _refs(),
      _versionMap() { }

FrontMem* SingleNut::getNut(const uint32_t version)
{
    return
        _versionMap.count(version)
            ? _versionMap[version]
            : NULL;
}

void SingleNut::setNut(const uint32_t version, FrontMem* s)
{
    _refs.checkout(s);
    _versionMap[version] = s;
}

}; // namespace chai_internal
