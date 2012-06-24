// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_JUST_IN_TIME_MEMO_HPP_
#define _CHAI_JUST_IN_TIME_MEMO_HPP_

#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "ArrayBuf.hpp"
#include "AstVariable.hpp"
#include "BackMem.hpp"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "MemManager.hpp"
#include "StmtIdSpace.hpp"
#include "StmtMatmulAuto.hpp"

namespace chai_internal {

////////////////////////////////////////
// memo and blackboard for the JIT

class JustInTimeMemo
{
    MemManager& _memMgr;

    // keep track of good kernels from autotuning
    std::map< std::string,      // exogenous key
              std::map< double, // GFLOPS
                        std::set< std::vector< size_t > > // kernel gen params
                      >
            > _goodAutoKernels;

    // keep track of bad kernels from autotuning
    std::set< std::string > _badAutoKernels;

    // either: varNum != -1 and varPtr == NULL
    //     or: varNum == -1 and varPtr != NULL
    typedef std::pair< uint32_t, const AstVariable* > VarKey;

    // variable vector lengths, determines images vs. memory buffers
    // vector of vector lengths is from highest GFLOPS to lowest
    std::map< VarKey, std::vector< size_t > > _varLength;
    std::set< std::vector< size_t > >         _rejectParams;

    // backdoor force variable vector length
    // 0 means image, 1|2|4 mean memory buffer of specified vector length
    // 7 means memory buffer of unspecified vector length
    std::map< uint32_t, size_t > _forceVarLength;

    VarKey astVarKey(const AstVariable*) const;

    size_t lookupVectorLength(const VarKey, const size_t precision);

    std::vector< size_t > autotuneLookup(const std::string& exoKey);
    bool autotuneSkip(const std::string& exoKey, bool& isFailure) const;
    void autotuneFail(const std::string& exoKey);

public:
    JustInTimeMemo(MemManager& memMgr);

    void clearTrace(void);

    size_t getVectorLength(const uint32_t varNum, const size_t precision);
    size_t getVectorLength(const AstVariable*);

    void forceVectorLength(const uint32_t varNum, const int vlen);

    std::vector< size_t > autotuneLookup(const Evergreen::MatmulMMBase&);
    std::vector< size_t > autotuneLookup(const Evergreen::MatmulMVBase&);

    bool autotuneSkip(const Evergreen::MatmulMMBase&, bool& isFailure) const;
    bool autotuneSkip(const Evergreen::MatmulMVBase&, bool& isFailure) const;
    bool autotuneGFLOPS(const Evergreen::MatmulMMBase&, const double GFLOPS);
    bool autotuneGFLOPS(const Evergreen::MatmulMVBase&, const double GFLOPS);
    void autotuneFail(const Evergreen::MatmulMMBase&);
    void autotuneFail(const Evergreen::MatmulMVBase&);
    bool autotuneVars(Evergreen::MatmulBase&,
                      const StmtMatmulAuto&,
                      const StmtIdSpace&);

    void eligibleGather(const AstVariable* dataVariable,
                        const StmtIdSpace&);

    bool createArrayTemp(const AstVariable*, StmtIdSpace&);
    bool createArrayTemp(const AstVariable*, StmtIdSpace&, const uint32_t);
    bool createArrayTemp(const AstVariable*, StmtIdSpace&, const int32_t);
    bool createArrayTemp(const AstVariable*, StmtIdSpace&, const float);
    bool createArrayTemp(const AstVariable*, StmtIdSpace&, const double);
    bool createArraySendData(const AstVariable*, StmtIdSpace&, BackMem*);
};

}; // namespace chai_internal

#endif
