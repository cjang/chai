// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_FUNCTION_HPP_
#define _CHAI_FUNCTION_HPP_

#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "Subscript.hpp"
#include "Variable.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// compute device function

class Function
{
    // unfortunately, OpenCL code is device dependent
    const size_t _deviceIndex;

    // function name for kernel
    const std::string _functionName;

    // function arguments
    std::vector< Variable* > _arguments;

    // private variables
    std::vector< Variable* > _privates;

    // trace variable arguments
    std::map< Variable*, uint32_t > _varTrace;
    std::map< uint32_t, Variable* > _traceVar;

    // split variable arguments
    std::map< Variable*, const AstVariable* > _varSplit;
    std::map< const AstVariable*, Variable* > _splitVar;

    // needed for: #pragma OPENCL EXTENSION cl_khr_fp64 : enable
    bool _fp64;

    // variables by type and role
    Sampler*              _sampler;
    std::set< Variable* > _typeImage2D;
    std::set< Variable* > _typeAddrMem;
    std::set< Variable* > _typeInteger;

    // temporary private variables
    std::map< std::pair< size_t, size_t >, AddrMem* > _privateVars;

    // private variables
    std::map< uint32_t, Variable* >           _tracePrivate;
    std::map< Variable*, uint32_t >           _privateTrace;
    std::map< const AstVariable*, Variable* > _splitPrivate;
    std::map< Variable*, const AstVariable* > _privateSplit;

    // variable name lookup (just makes them easier to read)
    std::map< const Variable*, size_t > _argumentLookup;
    std::map< const Variable*, size_t > _privateLookup;

    // body of function
    std::vector< std::string > _bodyText;

    // indentation
    size_t _indentStops;

    // for loop index variables
    size_t _loopIndexes;

    std::string makeName(const uint64_t hashCode,
                         const size_t kernelNumber) const;

    void addTraceArgument(const uint32_t, Variable*);
    void addSplitArgument(const AstVariable*, Variable*);

    void addPrivate(AddrMem*);
    void addPrivate(Integer*);

    void indentMore(void);
    void indentLess(void);
    std::ostream& indent(std::ostream&) const;

public:
    Function(const size_t deviceIndex,
             const uint64_t hashCode,
             const size_t kernelNumber);

    ~Function(void);

    std::string varNameLookup(const Variable*);
    std::string functionName(void) const;

    bool isFP64(void) const;

    void addArgument(const uint32_t, Image2D*);
    void addArgument(const uint32_t, AddrMem*);
    void addArgument(const uint32_t, Integer*);

    void addArgument(const AstVariable*, Image2D*);
    void addArgument(const AstVariable*, AddrMem*);
    void addArgument(const AstVariable*, Integer*);

    AddrMem* getPrivateVar(const size_t precision,
                           const size_t vectorLength);

    AddrMem* createPrivate(const uint32_t,
                           const size_t precision,
                           const size_t vectorLength);
    AddrMem* createPrivate(const AstVariable*,
                           const size_t precision,
                           const size_t vectorLength);

    const std::vector< Variable* >& arguments(void) const;

    uint32_t traceVar(Variable*);
    Variable* traceVar(const uint32_t);

    const AstVariable* splitVar(Variable*);
    Variable* splitVar(const AstVariable*);

    uint32_t traceVarPrivate(Variable*);
    Variable* traceVarPrivate(const uint32_t);

    const AstVariable* splitVarPrivate(Variable*);
    Variable* splitVarPrivate(const AstVariable*);

    bool isPrivate(Variable*) const;

    void assignZero(Variable* lhsVar);
    void assignZero(Variable* lhsVar,
                    Subscript* lhsSub);

    void assignNumber(Variable* lhsVar,
                      const double rhsVal);
    void assignNumber(Variable* lhsVar,
                      Subscript* lhsSub,
                      const double rhsVal);

    void assignVariables(Variable* lhsVar,
                         Variable* rhsVar);
    void assignVariables(Variable* lhsVar,
                         Subscript* lhsSub,
                         Variable* rhsVar);

    void assignStatement(Variable* lhsVar,
                         const std::string& rhsStmt);
    void assignStatement(Variable* lhsVar,
                         Subscript* lhsSub,
                         const std::string& rhsStmt);

    void incrementStatement(Variable* lhsVar,
                            const std::string& rhsStmt);
    void incrementStatement(Variable* lhsVar,
                            Subscript* lhsSub,
                            const std::string& rhsStmt);

    void barrier(void);

    size_t loopIndexes(void) const;
    size_t loopBegin(const size_t startIndex, const size_t upperLimit);
    void loopEnd(void);

    void print(std::vector< std::string >&);
};

}; // namespace chai_internal

#endif
