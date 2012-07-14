// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BaseAst.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// base of JIT AST streams

BaseAst::BaseAst(const size_t PREC,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 const bool randomness)
    : RefObj(),
      _prec(PREC),
      _W(W),
      _H(H),
      _slots(slots),
      _randomness(randomness),
      _refs(),
      _argAst() { }

void BaseAst::pushArg(BaseAst* obj)
{
    _argAst.push_back(obj);
    _refs.checkout(obj);
}

BaseAst::~BaseAst(void) { }

size_t BaseAst::W(void) const
{
    return _W;
}

size_t BaseAst::H(void) const
{
    return _H;
}

size_t BaseAst::slots(void) const
{
    return _slots;
}

size_t BaseAst::prec(void) const
{
    return _prec;
}

bool BaseAst::randomness(void) const
{
    return _randomness;
}

size_t BaseAst::numArg(void) const
{
    return _argAst.size();
}

BaseAst* BaseAst::getArg(const size_t idx) const
{
    return _argAst[idx];
}

void BaseAst::replaceArg(const size_t idx, BaseAst* obj)
{
    _argAst[idx] = obj;
    _refs.checkout(obj);
}

}; // namespace chai_internal
