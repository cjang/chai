// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "SourceKernel.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// compute kernel from source code

SourceKernel::SourceKernel(const string& functionName,
                           const vector< string >& sourceText,
                           const uint64_t sourceHashCode)
    : _functionName(functionName),
      _sourceText(sourceText),
      _sourceHashCode(sourceHashCode) { }

SourceKernel::~SourceKernel(void)
{
    for (vector< Variable* >::const_iterator
         it = _arguments.begin(); it != _arguments.end(); it++)
    {
        delete *it;
    }
}

string SourceKernel::functionName(void) const
{
    return _functionName;
}

const vector< Variable* >& SourceKernel::arguments(void) const
{
    return _arguments;
}

void SourceKernel::pushArgArray(const size_t PREC,
                                const uint32_t varNum)
{
    ArrayVariable* v = new ArrayVariable(PREC, varNum);

    _varTrace[ v ] = varNum;
    _traceVar[ varNum ] = v;

    _arguments.push_back( v );
}

void SourceKernel::pushArgLocal(const size_t PREC,
                                const size_t length)
{
    LocalMemory* v = new LocalMemory(PREC, length);

    _arguments.push_back( v );
}

void SourceKernel::pushArgScalar(const uint32_t a)
{
    ScalarArgument* v = new ScalarArgument(a);

    _arguments.push_back( v );
}

void SourceKernel::pushArgScalar(const int32_t a)
{
    ScalarArgument* v = new ScalarArgument(a);

    _arguments.push_back( v );
}

void SourceKernel::pushArgScalar(const float a)
{
    ScalarArgument* v = new ScalarArgument(a);

    _arguments.push_back( v );
}

void SourceKernel::pushArgScalar(const double a)
{
    ScalarArgument* v = new ScalarArgument(a);

    _arguments.push_back( v );
}

uint32_t SourceKernel::traceVar(Variable* obj)
{
    if (_varTrace.count(obj))
    {
        return _varTrace[ obj ];
    }
    else
    {
        return -1;
    }
}

Variable* SourceKernel::traceVar(const uint32_t variable)
{
    if (_traceVar.count(variable))
    {
        return _traceVar[ variable ];
    }
    else
    {
        return NULL;
    }
}

const AstVariable* SourceKernel::splitVar(Variable* obj)
{
    return NULL;
}

Variable* SourceKernel::splitVar(const AstVariable* variable)
{
    return NULL;
}

void SourceKernel::print(vector< string >& outText)
{
    for (vector< string >::const_iterator
         it = _sourceText.begin(); it != _sourceText.end(); it++)
    {
        outText.push_back( *it );
    }
}

}; // namespace chai_internal
