// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "EnqueueTrace.hpp"
#include "PrecType.hpp"
#include "StmtReadData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtReadData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    s.readoutPtr()->backMem()->swizzle();

    // continuation: need same standard vector length hack as everywhere else
    const FrontMem* proto = s.readoutPtr()->frontMem().front();

    // size of memory for each front memory box
    const size_t frontWidth = PrecType::padWidth(proto->precision(),
                                                 proto->W());
    const size_t frontSize
        = proto->H() * frontWidth * PrecType::sizeOf(proto->precision());

    // continuation: front memory without backing memory
    for (size_t i = 0; i < s.readoutPtr()->frontMem().size(); i++)
    {
        if (NULL == s.readoutPtr()->frontMem()[i]->ptrMem())
        {
            s.readoutPtr()
                ->frontMem()[i]
                ->setBackMem(
                      static_cast< char* >(s.readoutPtr()->backMem()->ptrMem())
                          + i * frontSize,
                      s.readoutPtr()->backMem() );
        }
    }
}

}; // namespace chai_internal
