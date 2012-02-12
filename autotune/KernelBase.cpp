// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <ostream>

#include "KernelBase.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// KernelBase

KernelBase::~KernelBase(void) { }

ostream& operator<< (ostream& os,
                     const KernelBase& kernelGen)
{
    return kernelGen.print(os);
}

}; // namespace chai_internal
