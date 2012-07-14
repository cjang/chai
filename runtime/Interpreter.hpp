// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERPRETER_HPP_
#define _CHAI_INTERPRETER_HPP_

#include <cstddef>
#include <stdint.h>

#include "BaseInterp.hpp"
#include "DeviceBase.hpp"
#include "DispatchInterp.hpp"
#include "InterpRNG.hpp"
#include "MemInterp.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// Interpreter controller

class Interpreter : public DeviceBase
{
    DispatchInterp _opDisp;

    InterpRNG _genRNG;

    void op(const uint32_t opCode,
            BaseInterp* opHandler);

    void internalOps(void);
    void arithmeticOps(void);
    void predicateOps(void);
    void gatherOps(void);
    void dataOps(void);
    void readOps(void);
    void randomOps(void);
    void matmulOps(void);
    void openclOps(void);
    void reduceOps(void);
    void mathOps(void);
    void commonOps(void);
    void integerOps(void);
    void relationalOps(void);

public:
    Interpreter(const int deviceNum);

    bool extendLanguage(const uint32_t opCode,
                        const BaseInterp& opHandler);

    void sub_initDevice(MemInterp&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
