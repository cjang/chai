// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
    MemManager*    _memMgr;

    InterpRNG _genRNG;

public:
    Interpreter(const size_t deviceCode);

    bool extendLanguage(const uint32_t opCode,
                        const BaseInterp& opHandler);

    void sub_initDevice(MemManager&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
