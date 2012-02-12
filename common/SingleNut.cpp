// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "SingleNut.hpp"

using namespace std;

namespace chai_internal {

SingleNut::SingleNut(void)
    : RefObj(),
      _refs(),
      _versionMem(),
      _versionVar(),
      _dummy() { }

const vector< FrontMem* >& SingleNut::getNutMem(const uint32_t version)
{
    return
        _versionMem.count(version)
            ? _versionMem[version]
            : _dummy;
}

void SingleNut::setNutMem(const uint32_t version, const vector< FrontMem* >& s)
{
    for (vector< FrontMem* >::const_iterator
         it = s.begin();
         it != s.end();
         it++)
    {
        _refs.checkout(*it);
    }

    _versionMem[version] = s;
}

AstVariable* SingleNut::getNutVar(const uint32_t version)
{
    if (_versionVar.count(version))
    {
        return _versionVar[version];
    }
    else
    {
        // Loop rolling removes many variable versions.
        // Return the highest one not greater than requested.
        // Failed JIT loop rolling for other devices may leave higher
        // versions behind as leftovers.

        for (map< uint32_t, AstVariable* >::const_reverse_iterator
             it = _versionVar.rbegin();
             it != _versionVar.rend();
             it++)
        {
            if ( (*it).first < version )
                return (*it).second;
        }

        // no suitable version found
        return NULL;
    }
}

void SingleNut::setNutVar(const uint32_t version, AstVariable* v)
{
    if (v)
    {
        _refs.checkout( (RefObj*)v ); // ugly cast
    }

    _versionVar[version] = v;
}

}; // namespace chai_internal
