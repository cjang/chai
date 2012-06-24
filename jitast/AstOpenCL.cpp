// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstOpenCL.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////////////
// inline OpenCL

AstOpenCL::AstOpenCL(const vector< string >& programSource,
                     const uint64_t programHashCode,
                     const string& kernelName)
    : BaseAst(-1, -1, -1),
      _programSource(programSource),
      _programHashCode(programHashCode),
      _kernelName(kernelName) { }

AstOpenCL::~AstOpenCL(void) { }

const vector< string >& AstOpenCL::programSource(void) const
{
    return _programSource;
}

uint64_t AstOpenCL::programHashCode(void) const
{
    return _programHashCode;
}

const string& AstOpenCL::kernelName(void) const
{
    return _kernelName;
}

size_t AstOpenCL::getWorkDim(void) const
{
    return _workGlobal.size();
}

size_t AstOpenCL::getGlobalWorkDim(const size_t i) const
{
    return (i < _workGlobal.size()) ? _workGlobal[i] : 0;
}

size_t AstOpenCL::getLocalWorkDim(const size_t i) const
{
    return (i < _workLocal.size()) ? _workLocal[i] : 0;
}

void AstOpenCL::setWorkSpace(const size_t global0,
                             const size_t local0)
{
    _workGlobal.push_back(global0);
    _workLocal.push_back(local0);
}

void AstOpenCL::setWorkSpace(const size_t global0,
                             const size_t global1,
                             const size_t local0,
                             const size_t local1)
{
    _workGlobal.push_back(global0);
    _workGlobal.push_back(global1);
    _workLocal.push_back(local0);
    _workLocal.push_back(local1);
}

void AstOpenCL::setArgArray(const size_t i,
                            const size_t precType,
                            const uint32_t varNum)
{
    _argKind[ i ]     = ARRAY_VARIABLE;
    _argPrecType[ i ] = precType;
    _argValue[ i ].u  = varNum;
}

void AstOpenCL::setArgLocal(const size_t i,
                            const size_t precType,
                            const size_t n)
{
    _argKind[ i ]     = LOCAL_MEM;
    _argPrecType[ i ] = precType;
    _argValue[ i ].s  = n;
}

void AstOpenCL::setArgScalar(const size_t i,
                             const size_t precType,
                             const uint32_t a)
{
    _argKind[ i ]     = SCALAR;
    _argPrecType[ i ] = precType;
    _argValue[ i ].u  = a;
}

void AstOpenCL::setArgScalar(const size_t i,
                             const size_t precType,
                             const int32_t a)
{
    _argKind[ i ]     = SCALAR;
    _argPrecType[ i ] = precType;
    _argValue[ i ].i  = a;
}

void AstOpenCL::setArgScalar(const size_t i,
                             const size_t precType,
                             const float a)
{
    _argKind[ i ]     = SCALAR;
    _argPrecType[ i ] = precType;
    _argValue[ i ].f  = a;
}

void AstOpenCL::setArgScalar(const size_t i,
                             const size_t precType,
                             const double a)
{
    _argKind[ i ]     = SCALAR;
    _argPrecType[ i ] = precType;
    _argValue[ i ].d  = a;
}

size_t AstOpenCL::getNumArgs(void) const
{
    set< size_t > s;

    for (map< size_t, ArgumentKind >::const_iterator
         it = _argKind.begin();
         it != _argKind.end();
         it++)
    {
        s.insert( (*it).first );
    }

    return s.size();
}

bool AstOpenCL::isArgArray(const size_t i)
{
    return ARRAY_VARIABLE == _argKind[ i ];
}

bool AstOpenCL::isArgLocal(const size_t i)
{
    return LOCAL_MEM == _argKind[ i ];
}

bool AstOpenCL::isArgScalar(const size_t i)
{
    return SCALAR == _argKind[ i ];
}

size_t AstOpenCL::getArgPrecType(const size_t i)
{
    return _argPrecType[ i ];
}

size_t AstOpenCL::getArgSizeT(const size_t i)
{
    return _argValue[ i ].s;
}

uint32_t AstOpenCL::getArgUInt32(const size_t i)
{
    return _argValue[ i ].u;
}

int32_t AstOpenCL::getArgInt32(const size_t i)
{
    return _argValue[ i ].i;
}

float AstOpenCL::getArgFloat(const size_t i)
{
    return _argValue[ i ].f;
}

double AstOpenCL::getArgDouble(const size_t i)
{
    return _argValue[ i ].d;
}

void AstOpenCL::accept(VisitAst& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
