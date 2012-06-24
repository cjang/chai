// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <stdarg.h>

#include "ArrayClient.hpp"
#include "AstOpenCL.hpp"
#include "ByteCodes.hpp"
#include "chai/chai.h"
#include "PrecType.hpp"
#include "SingleNut.hpp"
#include "TEA.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// inline OpenCL

void ProgramCL::initKeywords(void)
{
    _arrayBaseKeywords.insert( "__global" );
    _arrayBaseKeywords.insert( "global" );
    _arrayBaseKeywords.insert( "image2d_t" );

    _localMemKeywords.insert( "__local" );
    _localMemKeywords.insert( "local" );

    _scalarKeywords[ "unsigned" ] = PrecType::UInt32;
    _scalarKeywords[ "uint" ] = PrecType::UInt32;
    _scalarKeywords[ "int" ] = PrecType::Int32;
    _scalarKeywords[ "float" ] = PrecType::Float;
    _scalarKeywords[ "double" ] = PrecType::Double;
}

bool ProgramCL::parseKernelArgs(const string& strT)
{
    bool endOfArgs = false;

    vector< string > argTok;
    vector< size_t > argIdx;

    stringstream ss;

    for (size_t i = 0; !endOfArgs && i < strT.size(); i++)
    {
        switch (strT[i])
        {
            case ('(') :
                // ignore
                break;

            case (')') :
                argTok.push_back(ss.str());
                argIdx.push_back(_argIndex++);
                ss.str(string());
                endOfArgs = true;
                break;

            case (',') :
                argTok.push_back(ss.str());
                argIdx.push_back(_argIndex++);
                ss.str(string());
                break;

            default :
                if ('*' != strT[i]) ss << strT[i];
        }
    }

    if (! endOfArgs)
    {
        argTok.push_back(ss.str());
        argIdx.push_back(_argIndex);
    }

    for (size_t i = 0; i < argTok.size(); i++)
    {
        const string tok = argTok[i];
        const size_t idx = argIdx[i];

        if (_arrayBaseKeywords.count(tok))
        {
            _argKind[ idx ] = ARRAY_BASE;
        }
        else if (_localMemKeywords.count(tok))
        {
            _argKind[ idx ] = LOCAL_MEM;
        }
        else if (_scalarKeywords.count(tok))
        {
            if (0 == _argKind.count(idx))
            {
                _argKind[ idx ] = SCALAR;
            }

            _argPrecType[ idx ] = _scalarKeywords[ tok ];
        }
    }

    return ! endOfArgs;
}

void ProgramCL::setKernelArgs(const string& kernelName)
{
    bool argsOk = true;

    for (size_t i = 0; i < _argIndex; i++)
    {
        if (_argKind.count(i))
        {
            _kernelArgKind[ kernelName ].push_back( _argKind[i] );

            // image2d_t does not have a precision type in kernel source
            _kernelArgPrecType[ kernelName ].push_back( _argPrecType.count(i)
                                                            ? _argPrecType[i]
                                                            : -1 );
        }
        else
        {
            argsOk = false;
        }
    }

    if (! argsOk)
    {
        _kernelArgKind.erase( kernelName );
        _kernelArgPrecType.erase( kernelName );
    }
}

void ProgramCL::parseProgramText(void)
{
    bool kernelKey = false, voidKey = false;

    string kernelName;

    for (vector< string >::const_iterator
         it = _programText.begin();
         it != _programText.end();
         it++)
    {
        stringstream ss(*it);

        string strT;

        while (! ss.eof() && (ss >> strT))
        {
            if (kernelKey)
            {
                if (voidKey)
                {
                    if (! kernelName.empty())
                    {
                        if (! parseKernelArgs(strT))
                        {
                            setKernelArgs(kernelName);
                            kernelName.clear();
                            voidKey = kernelKey = false;
                        }
                    }
                    else
                    {
                        // read kernel name
                        _argIndex = 0;
                        _argKind.clear();
                        _argPrecType.clear();

                        const size_t end = strT.find("(");
                        kernelName = strT.substr(0, end);

                        if (string::npos != end &&
                            ! parseKernelArgs(strT.substr(end)))
                        {
                            setKernelArgs(kernelName);
                            kernelName.clear();
                            voidKey = kernelKey = false;
                        }
                    }
                }
                else if ("void" == strT) // no return value from kernel
                {
                    voidKey = true;
                }
            }
            else if ("__kernel" == strT || "kernel" == strT)
            {
                kernelKey = true;
            }
        }
    }
}

ProgramCL::Kernel::Kernel(ProgramCL& progCL,
                          const string& kernelName)
    : _progCL(progCL),
      _kernelName(kernelName),
      _astObj(new AstOpenCL(progCL._programText,
                            progCL._programHashCode,
                            kernelName)),
      _client(NULL),
      _refs(),
      _nut(new SingleNut),
      _argIndex(0)
{
    _refs.checkout(_nut);
    _refs.checkout(_astObj);
}

ProgramCL::Kernel::~Kernel(void)
{
    if (_client)
    {
        // ok
        const uint32_t variable = _client->variable();
        const uint32_t version  = 1;

        _client->constructor(variable, _nut);

        Stak<BC>& v = _client->assignment(variable, version);
        v.push( _client->frontMem(variable, _astObj) );
        v.push( ByteCodes::kernel_from_opencl );

        _client->destructor(variable);
    }
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const ArrayBase& arrayVar)
{
    if (! _client)
    {
        _client = arrayVar.getArrayClient();
    }

    _astObj->setArgArray(_argIndex++,
                         arrayVar.getArrayPrecType(),
                         arrayVar.getArrayVarNum());

    return *this;
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const size_t length)
{
    switch ( _progCL._kernelArgKind[ _kernelName ][ _argIndex ] )
    {
        case (LOCAL_MEM) :
            _astObj->setArgLocal(_argIndex++,
                                 _progCL._kernelArgPrecType[ _kernelName ]
                                                           [ _argIndex ],
                                 length);
            break;

        case (SCALAR) :
            _astObj->setArgScalar(_argIndex++,
                                  _progCL._kernelArgPrecType[ _kernelName ]
                                                            [ _argIndex ],
                                  static_cast<double>(length));
            break;
    }

    return *this;
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const uint32_t scalar)
{
    switch ( _progCL._kernelArgKind[ _kernelName ][ _argIndex ] )
    {
        case (LOCAL_MEM) :
            _astObj->setArgLocal(_argIndex++,
                                 _progCL._kernelArgPrecType[ _kernelName ]
                                                           [ _argIndex ],
                                 scalar);
            break;

        case (SCALAR) :
            _astObj->setArgScalar(_argIndex++,
                                  _progCL._kernelArgPrecType[ _kernelName ]
                                                            [ _argIndex ],
                                  scalar);
            break;
    }

    return *this;
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const int32_t scalar)
{
    switch ( _progCL._kernelArgKind[ _kernelName ][ _argIndex ] )
    {
        case (LOCAL_MEM) :
            _astObj->setArgLocal(_argIndex++,
                                 _progCL._kernelArgPrecType[ _kernelName ]
                                                           [ _argIndex ],
                                 scalar);
            break;

        case (SCALAR) :
            _astObj->setArgScalar(_argIndex++,
                                  _progCL._kernelArgPrecType[ _kernelName ]
                                                            [ _argIndex ],
                                  scalar);
            break;
    }

    return *this;
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const float scalar)
{
    switch ( _progCL._kernelArgKind[ _kernelName ][ _argIndex ] )
    {
        case (LOCAL_MEM) :
            _astObj->setArgLocal(_argIndex++,
                                 _progCL._kernelArgPrecType[ _kernelName ]
                                                           [ _argIndex ],
                                 scalar);
            break;

        case (SCALAR) :
            _astObj->setArgScalar(_argIndex++,
                                  _progCL._kernelArgPrecType[ _kernelName ]
                                                            [ _argIndex ],
                                  scalar);
            break;
    }

    return *this;
}

ProgramCL::Kernel& ProgramCL::Kernel::operator, (const double scalar)
{
    switch ( _progCL._kernelArgKind[ _kernelName ][ _argIndex ] )
    {
        case (LOCAL_MEM) :
            _astObj->setArgLocal(_argIndex++,
                                 _progCL._kernelArgPrecType[ _kernelName ]
                                                           [ _argIndex ],
                                 scalar);
            break;

        case (SCALAR) :
            _astObj->setArgScalar(_argIndex++,
                                  _progCL._kernelArgPrecType[ _kernelName ]
                                                            [ _argIndex ],
                                  scalar);
            break;
    }

    return *this;
}

void ProgramCL::Kernel::operator() (const size_t global0,
                                    const size_t local0)
{
    _astObj->setWorkSpace(global0, local0);
}

void ProgramCL::Kernel::operator() (const size_t global0,
                                    const size_t global1,
                                    const size_t local0,
                                    const size_t local1)
{
    _astObj->setWorkSpace(global0, global1, local0, local1);
}

ProgramCL::ProgramCL(const string& programText)
    : _programHashCode(TEA::hash(programText)),
      _programText()
{
    _programText.push_back( programText );

    initKeywords();
    parseProgramText();
}

ProgramCL::ProgramCL(const vector< string >& programText)
    : _programHashCode(TEA::hash(programText)),
      _programText(programText)
{
    initKeywords();
    parseProgramText();
}

ProgramCL::~ProgramCL(void) { }

ProgramCL::Kernel ProgramCL::operator, (const string& kernelName)
{
    return Kernel(*this, kernelName);
}

}; // namespace chai
