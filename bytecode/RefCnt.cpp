// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "RefCnt.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// reference counted base object

RefObj::RefObj(void)
    : _refcnt(1),
      _owned(false),
      _needMTSafe(false) { }

RefObj::RefObj(const bool needMTSafe)
    : _refcnt(1),
      _owned(false),
      _needMTSafe(needMTSafe)
{
    if (_needMTSafe)
        pthread_mutex_init(&_mtx, NULL);
}

RefObj::~RefObj(void)
{
    if (_needMTSafe)
        pthread_mutex_destroy(&_mtx);
}

void RefObj::needMTSafe(void)
{
    if (! _needMTSafe)
    {
        pthread_mutex_init(&_mtx, NULL);
        _needMTSafe = true;
    }
}

size_t RefObj::refcnt(void) const { return _refcnt; }

void RefObj::owned(void)
{
    if (! _owned) {
        _refcnt = 0;
        _owned = true;
    }
}

RefObj* RefObj::checkout(const bool owned)
{
    if (owned == _owned) _refcnt++;
    return this;
}

RefObj* RefObj::release(const bool owned)
{
    if (_needMTSafe)
        pthread_mutex_lock(&_mtx);

    if (owned == _owned) _refcnt--;
    RefObj* ptr = (0 == _refcnt) ? this : NULL;

    if (_needMTSafe)
        pthread_mutex_unlock(&_mtx);

    return ptr;
}

////////////////////////////////////////
// set of reference counted objects

void RefSet::deleteBag(void)
{
    for (set< RefObj* >::const_iterator
         it = _bag.begin(); it != _bag.end(); it++)
    {
        delete (*it)->release(true);
    }
}

RefSet::RefSet(void) { }

RefSet::~RefSet(void)
{
    deleteBag();
}

size_t RefSet::count(RefObj* obj) const
{
    return _bag.count(obj);
}

void RefSet::clear(void)
{
    deleteBag();
    _bag.clear();
}

void RefSet::checkout(RefObj* obj)
{
    obj->owned();
    if (0 == _bag.count(obj)) {
        _bag.insert(obj->checkout(true));
    }
}

void RefSet::checkout(const RefSet& other)
{
    for (set< RefObj* >::const_iterator
         it = other._bag.begin(); it != other._bag.end(); it++)
    {
        checkout(*it);
    }
}

void RefSet::release(RefObj* obj)
{
    _bag.erase(obj);
    delete obj->release(true);
}

void RefSet::release(const RefSet& other)
{
    for (set< RefObj* >::const_iterator
         it = other._bag.begin(); it != other._bag.end(); it++)
    {
        release(*it);
    }
}

}; // namespace chai_internal
