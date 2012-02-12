// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "VectorNut.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// read-from and write-back buffer to
// array variable nuts

VectorNut::VectorNut(void)
    : _nutVector() { }

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
        for (vector< FrontMem* >::const_iterator
             jt = (*it)->getNutMem(version).begin();
             jt != (*it)->getNutMem(version).end();
             jt++)
        {
            vec.push_back(*jt);
        }
    }

    return vec;
}

AstVariable* VectorNut::getNutVar(const uint32_t version)
{
    // retrieve from underlying array variable nuts
    return (*_nutVector.begin())->getNutVar(version);
}

void VectorNut::setNutMem(const uint32_t version, const vector< FrontMem* >& m)
{
    // no need to checkout references as the single nuts will do that

    if (1 == _nutVector.size())
    {
        // single thread and trace, possibly with vector array data
        _nutVector[0]->setNutMem(version, m);
    }
    else
    {
        // write-through to single nuts
        for (size_t i = 0; i < m.size(); i++)
        {
            vector< FrontMem* > v;
            v.push_back(m[i]);

            _nutVector[i]->setNutMem(version, v);
        }
    }
}

void VectorNut::setNutVar(const uint32_t version, AstVariable* v)
{
    // no need to checkout references as the single nuts will do that

    // write-through to single nuts
    for (vector< SingleNut* >::const_iterator
         it = _nutVector.begin();
         it != _nutVector.end();
         it++)
    {
        (*it)->setNutVar(version, v);
    }
}

}; // namespace chai_internal
