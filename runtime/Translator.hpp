// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANSLATOR_HPP_
#define _CHAI_TRANSLATOR_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>

#include "BaseTrans.hpp"
#include "DeviceBase.hpp"
#include "DispatchTrans.hpp"
#include "JustInTimeMemo.hpp"
#include "MemTrans.hpp"
#include "StandardEM.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// JIT controller

class Translator : public DeviceBase
{
    DispatchTrans _opDisp;

    StandardEM* _stdEM;

    std::map< uint64_t, JustInTimeMemo* > _jitMemo;

    void op(const uint32_t opCode,
            BaseTrans* opHandler);

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
    Translator(const size_t deviceCode);
    ~Translator(void);

    bool extendLanguage(const uint32_t opCode,
                        const BaseTrans& opHandler);

    void sub_initDevice(MemTrans&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
