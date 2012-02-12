// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANSLATOR_HPP_
#define _CHAI_TRANSLATOR_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>

#include "DeviceBase.hpp"
#include "DispatchTrans.hpp"
#include "JustInTimeMemo.hpp"
#include "MemManager.hpp"
#include "StandardEM.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class Translator : public DeviceBase
{
    DispatchTrans _opDisp;

    StandardEM* _stdEM;

    std::map< uint64_t, JustInTimeMemo* > _jitMemo;

public:
    Translator(const size_t deviceCode);
    ~Translator(void);

    bool extendLanguage(const uint32_t opCode,
                        const BaseTrans& opHandler);

    void sub_initDevice(MemManager&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
