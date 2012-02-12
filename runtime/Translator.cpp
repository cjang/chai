// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "BCStmtMaker.hpp"
#include "BCStmtTrace.hpp"
#include "ByteCodes.hpp"
#include "EvergreenMatmulBase.hpp"
#include "Logger.hpp"
#include "OCLhacks.hpp"
#include "TransAccum.hpp"
#include "TransCond.hpp"
#include "TransConvert.hpp"
#include "TransDotprod.hpp"
#include "TransFun1.hpp"
#include "TransFun2.hpp"
#include "TransFun3.hpp"
#include "TransGather.hpp"
#include "TransIdxdata.hpp"
#include "Translator.hpp"
#include "TransLitdata.hpp"
#include "TransMakedata.hpp"
#include "TransMatmul.hpp"
#include "TransMatmulG.hpp"
#include "TransReadout.hpp"
#include "TransRNGnormal.hpp"
#include "TransRNGuniform.hpp"
#include "TransScalar.hpp"
#include "TransTranspose.hpp"
#include "XStmtEnqueue.hpp"
#include "XStmtTrace.hpp"

using namespace std;

namespace chai_internal {

Translator::Translator(const size_t deviceCode)
    : DeviceBase(deviceCode),
      _stdEM(NULL),
      _jitMemo()
{
    // internal
    _opDisp.addOp(ByteCodes::convert_f32, new TransConvert(false));
    _opDisp.addOp(ByteCodes::convert_f64, new TransConvert(true));
    _opDisp.addOp(ByteCodes::scalar_f32, new TransScalar(false));
    _opDisp.addOp(ByteCodes::scalar_f64, new TransScalar(true));

    // arithmetic and predicate operators
    _opDisp.addOp(ByteCodes::negate, new TransFun1(_NEGATE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorADD, new TransFun2(_ADD_::singleton()));
    _opDisp.addOp(ByteCodes::operatorAND, new TransFun2(_AND_::singleton()));
    _opDisp.addOp(ByteCodes::operatorDIV, new TransFun2(_DIV_::singleton()));
    _opDisp.addOp(ByteCodes::operatorMUL, new TransFun2(_MUL_::singleton()));
    _opDisp.addOp(ByteCodes::operatorOR, new TransFun2(_OR_::singleton()));
    _opDisp.addOp(ByteCodes::operatorSUB, new TransFun2(_SUB_::singleton()));
    _opDisp.addOp(ByteCodes::operatorEQ, new TransFun2(_EQ_::singleton()));
    _opDisp.addOp(ByteCodes::operatorGE, new TransFun2(_GE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorGT, new TransFun2(_GT_::singleton()));
    _opDisp.addOp(ByteCodes::operatorLE, new TransFun2(_LE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorLT, new TransFun2(_LT_::singleton()));
    _opDisp.addOp(ByteCodes::operatorNE, new TransFun2(_NE_::singleton()));

    // selection and gathering
    _opDisp.addOp(ByteCodes::cond, new TransCond);
    _opDisp.addOp(ByteCodes::gather1_floor, new TransGather(1));
    _opDisp.addOp(ByteCodes::gather2_floor, new TransGather(2));
    _opDisp.addOp(ByteCodes::index1_f32, new TransIdxdata(1, false));
    _opDisp.addOp(ByteCodes::index1_f64, new TransIdxdata(1, true));
    _opDisp.addOp(ByteCodes::index2_f32, new TransIdxdata(2, false));
    _opDisp.addOp(ByteCodes::index2_f64, new TransIdxdata(2, true));

    // data creation
    _opDisp.addOp(ByteCodes::make1_f32, new TransMakedata(false));
    _opDisp.addOp(ByteCodes::make1_f64, new TransMakedata(true));
    _opDisp.addOp(ByteCodes::make2_f32, new TransMakedata(false));
    _opDisp.addOp(ByteCodes::make2_f64, new TransMakedata(true));
    const float floatOne = 1;
    const double doubleOne = 1;
    _opDisp.addOp(ByteCodes::ones1_f32, new TransLitdata(1, floatOne));
    _opDisp.addOp(ByteCodes::ones1_f64, new TransLitdata(1, doubleOne));
    _opDisp.addOp(ByteCodes::ones2_f32, new TransLitdata(2, floatOne));
    _opDisp.addOp(ByteCodes::ones2_f64, new TransLitdata(2, doubleOne));
    const float floatZero = 0;
    const double doubleZero = 0;
    _opDisp.addOp(ByteCodes::zeros1_f32, new TransLitdata(1, floatZero));
    _opDisp.addOp(ByteCodes::zeros1_f64, new TransLitdata(1, doubleZero));
    _opDisp.addOp(ByteCodes::zeros2_f32, new TransLitdata(2, floatZero));
    _opDisp.addOp(ByteCodes::zeros2_f64, new TransLitdata(2, doubleZero));

    // read back data
    _opDisp.addOp(ByteCodes::read_scalar_f32, new TransReadout(0));
    _opDisp.addOp(ByteCodes::read_scalar_f64, new TransReadout(0));
    _opDisp.addOp(ByteCodes::read1_f32, new TransReadout(1));
    _opDisp.addOp(ByteCodes::read1_f64, new TransReadout(1));
    _opDisp.addOp(ByteCodes::read2_f32, new TransReadout(2));
    _opDisp.addOp(ByteCodes::read2_f64, new TransReadout(2));

    // random numbers
    _opDisp.addOp(ByteCodes::rng_normal_make_f32, new TransRNGnormal(false));
    _opDisp.addOp(ByteCodes::rng_normal_make_f64, new TransRNGnormal(true));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f32, new TransRNGuniform(false));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f64, new TransRNGuniform(true));

    // auto-tuned matrix multiply
    _opDisp.addOp(ByteCodes::matmul, new TransMatmul);
    _opDisp.addOp(ByteCodes::matmulG, new TransMatmulG);
    _opDisp.addOp(ByteCodes::transpose, new TransTranspose);

    // reductions
    _opDisp.addOp(ByteCodes::dot_product, new TransDotprod);
    _opDisp.addOp(ByteCodes::mean, new TransAccum(true));
    _opDisp.addOp(ByteCodes::sum, new TransAccum(false));

    // math functions
    _opDisp.addOp(ByteCodes::acos, new TransFun1(_ACOS_::singleton()));
    _opDisp.addOp(ByteCodes::acosh, new TransFun1(_ACOSH_::singleton()));
    _opDisp.addOp(ByteCodes::acospi, new TransFun1(_ACOSPI_::singleton()));
    _opDisp.addOp(ByteCodes::asin, new TransFun1(_ASIN_::singleton()));
    _opDisp.addOp(ByteCodes::asinh, new TransFun1(_ASINH_::singleton()));
    _opDisp.addOp(ByteCodes::asinpi, new TransFun1(_ASINPI_::singleton()));
    _opDisp.addOp(ByteCodes::atan, new TransFun1(_ATAN_::singleton()));
    _opDisp.addOp(ByteCodes::atan2, new TransFun2(_ATAN2_::singleton()));
    _opDisp.addOp(ByteCodes::atanh, new TransFun1(_ATANH_::singleton()));
    _opDisp.addOp(ByteCodes::atanpi, new TransFun1(_ATANPI_::singleton()));
    _opDisp.addOp(ByteCodes::atan2pi, new TransFun2(_ATAN2PI_::singleton()));
    _opDisp.addOp(ByteCodes::cbrt, new TransFun1(_CBRT_::singleton()));
    _opDisp.addOp(ByteCodes::ceil, new TransFun1(_CEIL_::singleton()));
    _opDisp.addOp(ByteCodes::copysign, new TransFun2(_COPYSIGN_::singleton()));
    _opDisp.addOp(ByteCodes::cos, new TransFun1(_COS_::singleton()));
    _opDisp.addOp(ByteCodes::cosh, new TransFun1(_COSH_::singleton()));
    _opDisp.addOp(ByteCodes::cospi, new TransFun1(_COSPI_::singleton()));
    _opDisp.addOp(ByteCodes::erfc, new TransFun1(_ERFC_::singleton()));
    _opDisp.addOp(ByteCodes::erf, new TransFun1(_ERF_::singleton()));
    _opDisp.addOp(ByteCodes::exp, new TransFun1(_EXP_::singleton()));
    _opDisp.addOp(ByteCodes::exp2, new TransFun1(_EXP2_::singleton()));
    _opDisp.addOp(ByteCodes::exp10, new TransFun1(_EXP10_::singleton()));
    _opDisp.addOp(ByteCodes::expm1, new TransFun1(_EXPM1_::singleton()));
    _opDisp.addOp(ByteCodes::fabs, new TransFun1(_FABS_::singleton()));
    _opDisp.addOp(ByteCodes::fdim, new TransFun2(_FDIM_::singleton()));
    _opDisp.addOp(ByteCodes::floor, new TransFun1(_FLOOR_::singleton()));
    _opDisp.addOp(ByteCodes::fma, new TransFun3(_FMA_::singleton()));
    _opDisp.addOp(ByteCodes::fmax, new TransFun2(_FMAX_::singleton()));
    _opDisp.addOp(ByteCodes::fmin, new TransFun2(_FMIN_::singleton()));
    _opDisp.addOp(ByteCodes::fmod, new TransFun2(_FMOD_::singleton()));
    _opDisp.addOp(ByteCodes::hypot, new TransFun2(_HYPOT_::singleton()));
    _opDisp.addOp(ByteCodes::ilogb, new TransFun1(_ILOGB_::singleton()));
    _opDisp.addOp(ByteCodes::ldexp, new TransFun2(_LDEXP_::singleton()));
    _opDisp.addOp(ByteCodes::lgamma, new TransFun1(_LGAMMA_::singleton()));
    _opDisp.addOp(ByteCodes::log, new TransFun1(_LOG_::singleton()));
    _opDisp.addOp(ByteCodes::log2, new TransFun1(_LOG2_::singleton()));
    _opDisp.addOp(ByteCodes::log10, new TransFun1(_LOG10_::singleton()));
    _opDisp.addOp(ByteCodes::log1p, new TransFun1(_LOG1P_::singleton()));
    _opDisp.addOp(ByteCodes::logb, new TransFun1(_LOGB_::singleton()));
    _opDisp.addOp(ByteCodes::mad, new TransFun3(_MAD_::singleton()));
    _opDisp.addOp(ByteCodes::maxmag, new TransFun2(_MAXMAG_::singleton()));
    _opDisp.addOp(ByteCodes::minmag, new TransFun2(_MINMAG_::singleton()));
    _opDisp.addOp(ByteCodes::nan, new TransFun1(_NAN_::singleton()));
    _opDisp.addOp(ByteCodes::nextafter,
                  new TransFun2(_NEXTAFTER_::singleton()));
    _opDisp.addOp(ByteCodes::pow, new TransFun2(_POW_::singleton()));
    _opDisp.addOp(ByteCodes::pown, new TransFun2(_POWN_::singleton()));
    _opDisp.addOp(ByteCodes::powr, new TransFun2(_POWR_::singleton()));
    _opDisp.addOp(ByteCodes::remainder,
                  new TransFun2(_REMAINDER_::singleton()));
    _opDisp.addOp(ByteCodes::rint, new TransFun1(_RINT_::singleton()));
    _opDisp.addOp(ByteCodes::rootn, new TransFun2(_ROOTN_::singleton()));
    _opDisp.addOp(ByteCodes::round, new TransFun1(_ROUND_::singleton()));
    _opDisp.addOp(ByteCodes::rsqrt, new TransFun1(_RSQRT_::singleton()));
    _opDisp.addOp(ByteCodes::sin, new TransFun1(_SIN_::singleton()));
    _opDisp.addOp(ByteCodes::sinh, new TransFun1(_SINH_::singleton()));
    _opDisp.addOp(ByteCodes::sinpi, new TransFun1(_SINPI_::singleton()));
    _opDisp.addOp(ByteCodes::sqrt, new TransFun1(_SQRT_::singleton()));
    _opDisp.addOp(ByteCodes::tan, new TransFun1(_TAN_::singleton()));
    _opDisp.addOp(ByteCodes::tanh, new TransFun1(_TANH_::singleton()));
    _opDisp.addOp(ByteCodes::tanpi, new TransFun1(_TANPI_::singleton()));
    _opDisp.addOp(ByteCodes::tgamma, new TransFun1(_TGAMMA_::singleton()));
    _opDisp.addOp(ByteCodes::trunc, new TransFun1(_TRUNC_::singleton()));

    // common functions
    _opDisp.addOp(ByteCodes::clamp, new TransFun3(_CLAMP_::singleton()));
    _opDisp.addOp(ByteCodes::degrees, new TransFun1(_DEGREES_::singleton()));
    _opDisp.addOp(ByteCodes::max, new TransFun2(_MAX_::singleton()));
    _opDisp.addOp(ByteCodes::min, new TransFun2(_MIN_::singleton()));
    _opDisp.addOp(ByteCodes::mix, new TransFun3(_MIX_::singleton()));
    _opDisp.addOp(ByteCodes::radians, new TransFun1(_RADIANS_::singleton()));
    _opDisp.addOp(ByteCodes::step, new TransFun2(_STEP_::singleton()));
    _opDisp.addOp(ByteCodes::smoothstep,
                  new TransFun3(_SMOOTHSTEP_::singleton()));
    _opDisp.addOp(ByteCodes::sign, new TransFun1(_SIGN_::singleton()));

    // relational functions
    _opDisp.addOp(ByteCodes::isequal, new TransFun2(_ISEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isnotequal,
                  new TransFun2(_ISNOTEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isgreater,
                  new TransFun2(_ISGREATER_::singleton()));
    _opDisp.addOp(ByteCodes::isgreaterequal,
                  new TransFun2(_ISGREATEREQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isless, new TransFun2(_ISLESS_::singleton()));
    _opDisp.addOp(ByteCodes::islessequal,
                  new TransFun2(_ISLESSEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::islessgreater,
                  new TransFun2(_ISLESSGREATER_::singleton()));
    _opDisp.addOp(ByteCodes::isfinite, new TransFun1(_ISFINITE_::singleton()));
    _opDisp.addOp(ByteCodes::isinf, new TransFun1(_ISINF_::singleton()));
    _opDisp.addOp(ByteCodes::isnan, new TransFun1(_ISNAN_::singleton()));
    _opDisp.addOp(ByteCodes::isnormal, new TransFun1(_ISNORMAL_::singleton()));
    _opDisp.addOp(ByteCodes::isordered,
                  new TransFun2(_ISORDERED_::singleton()));
    _opDisp.addOp(ByteCodes::isunordered,
                  new TransFun2(_ISUNORDERED_::singleton()));
    _opDisp.addOp(ByteCodes::signbit, new TransFun1(_SIGNBIT_::singleton()));
}

Translator::~Translator(void)
{
    delete _stdEM;

    set< JustInTimeMemo* > memoSet;

    for (map< uint64_t, JustInTimeMemo* >::const_iterator
         it = _jitMemo.begin();
         it != _jitMemo.end();
         it++)
    {
        memoSet.insert((*it).second);
    }

    for (set< JustInTimeMemo* >::const_iterator
         it = memoSet.begin();
         it != memoSet.end();
         it++)
    {
        delete *it;
    }
}

bool Translator::extendLanguage(const uint32_t opCode,
                                const BaseTrans& opHandler)
{
    _opDisp.deleteOp(opCode); // release old handler, if any
    _opDisp.addOp(opCode, opHandler.clone()); // replace with new handler

    _opDisp.setContext(getMemManager());

    return true;
}

void Translator::sub_initDevice(MemManager& mm)
{
    _opDisp.setContext(mm);

    const size_t deviceIdx = getDeviceIndex();
    const size_t numSearch = OCLhacks::singleton().searchTrials(deviceIdx);
    const size_t numTiming = OCLhacks::singleton().timingTrials(deviceIdx);

    _stdEM = new StandardEM(mm.getComputeDevice());
    _stdEM->setNumberTrials(numSearch, numTiming);
    _stdEM->setJournalFile(OCLhacks::singleton().journalFile(deviceIdx));
}

bool Translator::sub_evaluate(VectorTrace& vt)
{
    _opDisp.setContext(vt);

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "deviceCode=" << getDeviceCode();
    LOGGER(ss.str())
#endif

    BCStmtTrace bcsTrace(vt);

    // bytecode code transformations
    bcsTrace.optimizeBytecode();

    // generate boxed statements
    BCStmtMaker bcsMaker(_opDisp);
    bcsTrace.traverse(bcsMaker);

    // working trace
    XStmtTrace xsTrace(bcsTrace);

    // perform reorder and together
    xsTrace.reorder();
    xsTrace.together();

    // unique just-in-time memo for each continued trace
    JustInTimeMemo* jitMemo = NULL;
    for (vector< uint64_t >::const_iterator
         it = vt.hashCodeHistory().begin();
         it != vt.hashCodeHistory().end();
         it++)
    {
        const uint64_t hashCode = *it;
        if (0 == _jitMemo.count(hashCode))
        {
            if (NULL == jitMemo)
                jitMemo = new JustInTimeMemo(getMemManager());

            _jitMemo[hashCode] = jitMemo;
        }
        else
        {
            jitMemo = _jitMemo[hashCode];
        }
    }

    // enqueue
    XStmtEnqueue xsEnqueue(vt, getMemManager(), *_stdEM, *jitMemo);
    xsTrace.traverse(xsEnqueue);

    return xsEnqueue.isOk();
}

}; // namespace chai_internal
