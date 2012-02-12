// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EXT_BYTE_CODES_HPP_
#define _CHAI_EXT_BYTE_CODES_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// language extension operation bytecodes

namespace ByteCodes {

    // start codes at 1000 to avoid any confusion with core platform opcodes

    ////////////////////////////////////////
    // no operation, just an example

    static const uint32_t nop                     = 1000;

}; // namespace ByteCodes

}; // namespace chai_internal

#endif
