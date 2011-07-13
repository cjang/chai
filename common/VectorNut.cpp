// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "VectorNut.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read-from and write-back buffer to
// array variable nuts

VectorNut::VectorNut(void)
    : _refs(),
      _nutVector(),
      _versionMap() { }

void VectorNut::push(SingleNut* nut)
{
    _nutVector.push_back(nut);
}

vector< FrontMem* > VectorNut::getNutMem(const uint32_t version)
{
    vector< FrontMem* > vec;

    // retrieve from underlying array variable nuts
    for (vector< SingleNut* >::const_iterator
         it = _nutVector.begin();
         it != _nutVector.end();
         it++)
    {
        FrontMem* fm = (*it)->getNut(version);

        // if the version does not exist, an empty vector is returned
        if (NULL == fm)
            break;

        vec.push_back(fm);
    }

    return vec;
}

AstVariable* VectorNut::getNutVar(const uint32_t version)
{
    if (_versionMap.count(version))
    {
        return _versionMap[version];
    }
    else
    {
        // Loop rolling removes many variable versions.
        // Return the highest one not greater than requested.
        // Failed JIT loop rolling for other devices may leave higher
        // versions behind as leftovers.

        for (map< uint32_t, AstVariable* >::const_reverse_iterator
             it = _versionMap.rbegin();
             it != _versionMap.rend();
             it++)
        {
            if ( (*it).first < version )
                return (*it).second;
        }

        // no suitable version found
        return NULL;
    }
}

void VectorNut::setNut(const uint32_t version, const vector< FrontMem* >& m)
{
    // no need to checkout references as the single nuts will do that

    // write-through to single nuts
    for (size_t i = 0; i < m.size(); i++)
    {
        _nutVector[i]->setNut(version, m[i]);
    }
}

void VectorNut::setNut(const uint32_t version, AstVariable* obj)
{
    if (obj)
    {
        _refs.checkout( (RefObj*)obj ); // ugly cast
    }

    _versionMap[version] = obj;
}

}; // namespace chai_internal
