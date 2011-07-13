// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERPRETER_HPP_
#define _CHAI_INTERPRETER_HPP_

#include "DeviceBase.hpp"
#include "DispatchInterp.hpp"
#include "InterpRNG.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class Interpreter : public DeviceBase
{
    DispatchInterp _opDisp;

    InterpRNG  _genRNG;

public:
    Interpreter(const size_t deviceCode);

    void sub_initDevice(MemManager&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
