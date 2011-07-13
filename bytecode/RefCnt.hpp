// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_REF_CNT_HPP_
#define _CHAI_REF_CNT_HPP_

#include <pthread.h>
#include <set>

namespace chai_internal {

////////////////////////////////////////
// reference counted base object

class RefObj
{
    size_t _refcnt;
    bool   _owned;

    bool            _needMTSafe;
    pthread_mutex_t _mtx;

protected:
    RefObj(void);
    RefObj(const bool needMTSafe);

public:
    virtual ~RefObj(void);

    void needMTSafe(void);

    size_t refcnt(void) const;

    void owned(void);

    RefObj* checkout(const bool owned = false);
    RefObj* release(const bool owned = false);
};

////////////////////////////////////////
// set of reference counted objects

class RefSet
{
    std::set< RefObj* > _bag;

    void deleteBag(void);

public:
    RefSet(void);
    ~RefSet(void);

    // undefined
    RefSet(const RefSet&);
    RefSet& operator= (const RefSet&);

    size_t count(RefObj*) const;

    void clear(void);

    void checkout(RefObj*);
    void checkout(const RefSet&);

    void release(RefObj*);
    void release(const RefSet&);
};

}; // namespace chai_internal

#endif
