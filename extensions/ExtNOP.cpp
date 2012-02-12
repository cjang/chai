// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <sstream>
#include <string>

#include "ExtNOP.hpp"
#include "Logger.hpp"
#include "OCLdevice.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// language extension object

ExtNOP::ExtNOP(BaseAst* barg)
    : AstExtension(barg->W(), barg->H(), barg->isDouble())
{
    pushArg(barg);
}

void ExtNOP::accept(VisitAst& v)
{
    v.visit(*this);
}

string ExtNOP::extensionName(void) const
{
    return "nop";
}

bool ExtNOP::extensionEnqueue(MemManager& memMgr,
                              VectorTrace& vt,
                              AstVariable* lhs)
{
    ArrayBuf* A = lhs->isTraceVariable()
                      ? memMgr.getArrayBuf(lhs->variable(), vt)
                      : memMgr.getArrayBuf(lhs, vt);

    AstVariable* rhs = static_cast< AstVariable* >(getArg(0));

    ArrayBuf* B = rhs->isTraceVariable()
                      ? memMgr.getArrayBuf(rhs->variable(), vt)
                      : memMgr.getArrayBuf(rhs, vt);

    const string kernelName = "nop";

    stringstream ss;

    if (lhs->isDouble() || rhs->isDouble())
        ss << "#pragma OPENCL EXTENSION cl_amd_fp64 : enable" << endl;

    ss << "__kernel void " << kernelName
       << "(__global " << (rhs->isDouble() ? "double" : "float") << "* A,"
          " __global const " << (lhs->isDouble() ? "double" : "float") << "* B)"
       << endl
       << "{" << endl
       << "A[ get_global_id(0) ] = B[ get_global_id(0) ];" << endl
       << "}";

    LOGGER(ss.str())

    OCLkernel ckernel(memMgr.getComputeDevice(),
                      kernelName,
                      ss.str());

    A->argumentToKernel(ckernel);
    B->argumentToKernel(ckernel);

    A->flushToDevice();
    B->flushToDevice();

    ckernel << OCLWorkIndex(lhs->W() * lhs->H(), 1);

    memMgr.getComputeDevice() << ckernel;

    memMgr.getComputeDevice() << FLUSH;

    return true;
}

bool ExtNOP::extensionEnqueueAuto(MemManager& memMgr,
                                  VectorTrace& vt,
                                  AstVariable* lhs)
{
    // no auto-tuning to do for this simple example
    return true;
}

}; // namespace chai_internal
