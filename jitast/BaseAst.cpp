// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BaseAst.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// base of JIT AST streams

BaseAst::BaseAst(const size_t W,
                 const size_t H,
                 const size_t precision)
    : RefObj(),
      _W(W),
      _H(H),
      _precision(precision),
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

size_t BaseAst::precision(void) const
{
    return _precision;
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
