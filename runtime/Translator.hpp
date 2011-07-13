// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TRANSLATOR_HPP_
#define _CHAI_TRANSLATOR_HPP_

#include <cstddef>

#include "DeviceBase.hpp"
#include "DispatchTrans.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

class Translator : public DeviceBase
{
    DispatchTrans _opDisp;

public:
    Translator(const size_t deviceCode);

    void sub_initDevice(MemManager&);
    bool sub_evaluate(VectorTrace&);
};

}; // namespace chai_internal

#endif
