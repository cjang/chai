// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ArrayClient.hpp"
#include "chai/chai.h"
#include "Scheduler.hpp"
#include "SingleNut.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// array variable abstract base class

void ArrayBase::initClient(void)
{
    if (! _client)
    {
        _client = Scheduler::singleton().client();
        _variable = _client->variable();
        _version = 1;
        _client->constructor(_variable, _nut);
    }
}

void ArrayBase::setArrayDim(const size_t W,
                            const size_t H,
                            const size_t slots)
{
    // width and height set on creation, no resizing allowed
    if (! _slots)
    {
        _W = W;
        _H = H;
        _slots = slots;
    }

    // data parallel can always expand
    if (slots > _slots)
        _slots = slots;
}

ArrayBase::ArrayBase(const size_t PREC)
    : _prec(PREC),
      _client(NULL),
      _variable(-1),
      _version(-1),
      _refs(),
      _nut(new SingleNut),
      _W(0),
      _H(0),
      _slots(0)
{
    _refs.checkout(_nut);
}

ArrayBase::ArrayBase(const size_t PREC,
                     const size_t W,
                     const size_t H,
                     const size_t slots)
    : _prec(PREC),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(W),
      _H(H),
      _slots(slots)
{
    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::ArrayBase(const ArrayBase& other)
    : _prec(other._prec),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(other._W),
      _H(other._H),
      _slots(other._slots)
{
    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::ArrayBase(const size_t PREC,
                     const ArrayExp& rhs)
    : _prec(PREC),
      _client(Scheduler::singleton().client()),
      _variable(_client->variable()),
      _version(1),
      _refs(),
      _nut(new SingleNut),
      _W(0),
      _H(0),
      _slots(0)
{
    stack< ArrayDim > dimStack;
    rhs.accept(dimStack);

    const ArrayDim& t = dimStack.top();

    setArrayDim(t.width,
                t.height,
                t.slots);

    _refs.checkout(_nut);
    _client->constructor(_variable, _nut);
}

ArrayBase::~ArrayBase(void)
{
    if (_client)
        _client->destructor(_variable);
}

ArrayBase& ArrayBase::assignOther(const ArrayBase& other)
{
    initClient();

    setArrayDim(other._W,
                other._H,
                other._slots);
}

ArrayBase& ArrayBase::assignExp(const ArrayExp& rhs)
{
    initClient();

    stack< ArrayDim > dimStack;
    rhs.accept(dimStack);

    const ArrayDim& t = dimStack.top();

    setArrayDim(t.width,
                t.height,
                t.slots);
}

ArrayClient& ArrayBase::client(void) const
{
    return *_client;
}

uint32_t ArrayBase::variable(void) const
{
    return _variable;
}

uint32_t ArrayBase::version(void) const
{
    return _version;
}

uint32_t ArrayBase::versionInc(void)
{
    return _version++;
}

size_t ArrayBase::W(void) const
{
    return _W;
}

size_t ArrayBase::H(void) const
{
    return _H;
}

size_t ArrayBase::slots(void) const
{
    return _slots;
}

ArrayDim ArrayBase::arrayDim(void) const
{
    return ArrayDim(_W, _H, _slots);
}

ArrayClient* ArrayBase::getArrayClient(void) const
{
    return _client;
}

size_t ArrayBase::getArrayPrec(void) const
{
    return _prec;
}

uint32_t ArrayBase::getArrayVarNum(void) const
{
    return _variable;
}

}; // namespace chai
