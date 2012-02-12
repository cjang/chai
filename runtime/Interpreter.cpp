// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "ByteCodes.hpp"
#include "InterpAccum.hpp"
#include "InterpCond.hpp"
#include "InterpConvert.hpp"
#include "InterpDotprod.hpp"
#include "InterpGather.hpp"
#include "InterpIdxdata.hpp"
#include "InterpFun1.hpp"
#include "InterpFun2.hpp"
#include "InterpFun3.hpp"
#include "InterpLitdata.hpp"
#include "InterpMakedata.hpp"
#include "InterpMatmul.hpp"
#include "InterpMatmulG.hpp"
#include "InterpReadout.hpp"
#include "InterpRNG.hpp"
#include "InterpRNGnormal.hpp"
#include "InterpRNGuniform.hpp"
#include "InterpScalar.hpp"
#include "InterpTranspose.hpp"
#include "Interpreter.hpp"
#include "Logger.hpp"
#include "MemManager.hpp"
#include "PrintBC.hpp"
#include "Scheduler.hpp"

using namespace std;

namespace chai_internal {

Interpreter::Interpreter(const size_t deviceCode)
    : DeviceBase(deviceCode),
      _genRNG()
{
    // internal
    _opDisp.addOp(ByteCodes::convert_f32, new InterpConvert(false));
    _opDisp.addOp(ByteCodes::convert_f64, new InterpConvert(true));
    _opDisp.addOp(ByteCodes::scalar_f32, new InterpScalar(false));
    _opDisp.addOp(ByteCodes::scalar_f64, new InterpScalar(true));

    // arithmetic and predicate operators
    _opDisp.addOp(ByteCodes::negate, new InterpFun1(_NEGATE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorADD, new InterpFun2(_ADD_::singleton()));
    _opDisp.addOp(ByteCodes::operatorAND, new InterpFun2(_AND_::singleton()));
    _opDisp.addOp(ByteCodes::operatorDIV, new InterpFun2(_DIV_::singleton()));
    _opDisp.addOp(ByteCodes::operatorMUL, new InterpFun2(_MUL_::singleton()));
    _opDisp.addOp(ByteCodes::operatorOR, new InterpFun2(_OR_::singleton()));
    _opDisp.addOp(ByteCodes::operatorSUB, new InterpFun2(_SUB_::singleton()));
    _opDisp.addOp(ByteCodes::operatorEQ, new InterpFun2(_EQ_::singleton()));
    _opDisp.addOp(ByteCodes::operatorGE, new InterpFun2(_GE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorGT, new InterpFun2(_GT_::singleton()));
    _opDisp.addOp(ByteCodes::operatorLE, new InterpFun2(_LE_::singleton()));
    _opDisp.addOp(ByteCodes::operatorLT, new InterpFun2(_LT_::singleton()));
    _opDisp.addOp(ByteCodes::operatorNE, new InterpFun2(_NE_::singleton()));

    // selection and gathering
    _opDisp.addOp(ByteCodes::cond, new InterpCond);
    _opDisp.addOp(ByteCodes::gather1_floor, new InterpGather(1));
    _opDisp.addOp(ByteCodes::gather2_floor, new InterpGather(2));
    _opDisp.addOp(ByteCodes::index1_f32, new InterpIdxdata(false, 1));
    _opDisp.addOp(ByteCodes::index1_f64, new InterpIdxdata(true, 1));
    _opDisp.addOp(ByteCodes::index2_f32, new InterpIdxdata(false, 2));
    _opDisp.addOp(ByteCodes::index2_f64, new InterpIdxdata(true, 2));

    // data creation
    _opDisp.addOp(ByteCodes::make1_f32, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make1_f64, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make2_f32, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make2_f64, new InterpMakedata);
    const float floatOne = 1;
    const double doubleOne = 1;
    _opDisp.addOp(ByteCodes::ones1_f32, new InterpLitdata(floatOne, 1));
    _opDisp.addOp(ByteCodes::ones1_f64, new InterpLitdata(doubleOne, 1));
    _opDisp.addOp(ByteCodes::ones2_f32, new InterpLitdata(floatOne, 2));
    _opDisp.addOp(ByteCodes::ones2_f64, new InterpLitdata(doubleOne, 2));
    const float floatZero = 0;
    const double doubleZero = 0;
    _opDisp.addOp(ByteCodes::zeros1_f32, new InterpLitdata(floatZero, 1));
    _opDisp.addOp(ByteCodes::zeros1_f64, new InterpLitdata(doubleZero, 1));
    _opDisp.addOp(ByteCodes::zeros2_f32, new InterpLitdata(floatZero, 2));
    _opDisp.addOp(ByteCodes::zeros2_f64, new InterpLitdata(doubleZero, 2));

    // read back data
    _opDisp.addOp(ByteCodes::read_scalar_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read_scalar_f64, new InterpReadout);
    _opDisp.addOp(ByteCodes::read1_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read1_f64, new InterpReadout);
    _opDisp.addOp(ByteCodes::read2_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read2_f64, new InterpReadout);

    // random numbers
    _opDisp.addOp(ByteCodes::rng_normal_make_f32,
                  new InterpRNGnormal(false, _genRNG));
    _opDisp.addOp(ByteCodes::rng_normal_make_f64,
                  new InterpRNGnormal(true, _genRNG));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f32,
                  new InterpRNGuniform(false, _genRNG));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f64,
                  new InterpRNGuniform(true, _genRNG));

    // auto-tuned matrix multiply
    _opDisp.addOp(ByteCodes::matmul, new InterpMatmul);
    _opDisp.addOp(ByteCodes::matmulG, new InterpMatmulG);
    _opDisp.addOp(ByteCodes::transpose, new InterpTranspose);

    // reductions
    _opDisp.addOp(ByteCodes::dot_product, new InterpDotprod);
    _opDisp.addOp(ByteCodes::mean, new InterpAccum(true));
    _opDisp.addOp(ByteCodes::sum, new InterpAccum(false));

    // math functions
    _opDisp.addOp(ByteCodes::acos, new InterpFun1(_ACOS_::singleton()));
    _opDisp.addOp(ByteCodes::acosh, new InterpFun1(_ACOSH_::singleton()));
    _opDisp.addOp(ByteCodes::acospi, new InterpFun1(_ACOSPI_::singleton()));
    _opDisp.addOp(ByteCodes::asin, new InterpFun1(_ASIN_::singleton()));
    _opDisp.addOp(ByteCodes::asinh, new InterpFun1(_ASINH_::singleton()));
    _opDisp.addOp(ByteCodes::asinpi, new InterpFun1(_ASINPI_::singleton()));
    _opDisp.addOp(ByteCodes::atan, new InterpFun1(_ATAN_::singleton()));
    _opDisp.addOp(ByteCodes::atan2, new InterpFun2(_ATAN2_::singleton()));
    _opDisp.addOp(ByteCodes::atanh, new InterpFun1(_ATANH_::singleton()));
    _opDisp.addOp(ByteCodes::atanpi, new InterpFun1(_ATANPI_::singleton()));
    _opDisp.addOp(ByteCodes::atan2pi, new InterpFun2(_ATAN2PI_::singleton()));
    _opDisp.addOp(ByteCodes::cbrt, new InterpFun1(_CBRT_::singleton()));
    _opDisp.addOp(ByteCodes::ceil, new InterpFun1(_CEIL_::singleton()));
    _opDisp.addOp(ByteCodes::copysign, new InterpFun2(_COPYSIGN_::singleton()));
    _opDisp.addOp(ByteCodes::cos, new InterpFun1(_COS_::singleton()));
    _opDisp.addOp(ByteCodes::cosh, new InterpFun1(_COSH_::singleton()));
    _opDisp.addOp(ByteCodes::cospi, new InterpFun1(_COSPI_::singleton()));
    _opDisp.addOp(ByteCodes::erfc, new InterpFun1(_ERFC_::singleton()));
    _opDisp.addOp(ByteCodes::erf, new InterpFun1(_ERF_::singleton()));
    _opDisp.addOp(ByteCodes::exp, new InterpFun1(_EXP_::singleton()));
    _opDisp.addOp(ByteCodes::exp2, new InterpFun1(_EXP2_::singleton()));
    _opDisp.addOp(ByteCodes::exp10, new InterpFun1(_EXP10_::singleton()));
    _opDisp.addOp(ByteCodes::expm1, new InterpFun1(_EXPM1_::singleton()));
    _opDisp.addOp(ByteCodes::fabs, new InterpFun1(_FABS_::singleton()));
    _opDisp.addOp(ByteCodes::fdim, new InterpFun2(_FDIM_::singleton()));
    _opDisp.addOp(ByteCodes::floor, new InterpFun1(_FLOOR_::singleton()));
    _opDisp.addOp(ByteCodes::fma, new InterpFun3(_FMA_::singleton()));
    _opDisp.addOp(ByteCodes::fmax, new InterpFun2(_FMAX_::singleton()));
    _opDisp.addOp(ByteCodes::fmin, new InterpFun2(_FMIN_::singleton()));
    _opDisp.addOp(ByteCodes::fmod, new InterpFun2(_FMOD_::singleton()));
    _opDisp.addOp(ByteCodes::hypot, new InterpFun2(_HYPOT_::singleton()));
    _opDisp.addOp(ByteCodes::ilogb, new InterpFun1(_ILOGB_::singleton()));
    _opDisp.addOp(ByteCodes::ldexp, new InterpFun2(_LDEXP_::singleton()));
    _opDisp.addOp(ByteCodes::lgamma, new InterpFun1(_LGAMMA_::singleton()));
    _opDisp.addOp(ByteCodes::log, new InterpFun1(_LOG_::singleton()));
    _opDisp.addOp(ByteCodes::log2, new InterpFun1(_LOG2_::singleton()));
    _opDisp.addOp(ByteCodes::log10, new InterpFun1(_LOG10_::singleton()));
    _opDisp.addOp(ByteCodes::log1p, new InterpFun1(_LOG1P_::singleton()));
    _opDisp.addOp(ByteCodes::logb, new InterpFun1(_LOGB_::singleton()));
    _opDisp.addOp(ByteCodes::mad, new InterpFun3(_MAD_::singleton()));
    _opDisp.addOp(ByteCodes::maxmag, new InterpFun2(_MAXMAG_::singleton()));
    _opDisp.addOp(ByteCodes::minmag, new InterpFun2(_MINMAG_::singleton()));
    _opDisp.addOp(ByteCodes::nan, new InterpFun1(_NAN_::singleton()));
    _opDisp.addOp(ByteCodes::nextafter,
                  new InterpFun2(_NEXTAFTER_::singleton()));
    _opDisp.addOp(ByteCodes::pow, new InterpFun2(_POW_::singleton()));
    _opDisp.addOp(ByteCodes::pown, new InterpFun2(_POWN_::singleton()));
    _opDisp.addOp(ByteCodes::powr, new InterpFun2(_POWR_::singleton()));
    _opDisp.addOp(ByteCodes::remainder,
                  new InterpFun2(_REMAINDER_::singleton()));
    _opDisp.addOp(ByteCodes::rint, new InterpFun1(_RINT_::singleton()));
    _opDisp.addOp(ByteCodes::rootn, new InterpFun2(_ROOTN_::singleton()));
    _opDisp.addOp(ByteCodes::round, new InterpFun1(_ROUND_::singleton()));
    _opDisp.addOp(ByteCodes::rsqrt, new InterpFun1(_RSQRT_::singleton()));
    _opDisp.addOp(ByteCodes::sin, new InterpFun1(_SIN_::singleton()));
    _opDisp.addOp(ByteCodes::sinh, new InterpFun1(_SINH_::singleton()));
    _opDisp.addOp(ByteCodes::sinpi, new InterpFun1(_SINPI_::singleton()));
    _opDisp.addOp(ByteCodes::sqrt, new InterpFun1(_SQRT_::singleton()));
    _opDisp.addOp(ByteCodes::tan, new InterpFun1(_TAN_::singleton()));
    _opDisp.addOp(ByteCodes::tanh, new InterpFun1(_TANH_::singleton()));
    _opDisp.addOp(ByteCodes::tanpi, new InterpFun1(_TANPI_::singleton()));
    _opDisp.addOp(ByteCodes::tgamma, new InterpFun1(_TGAMMA_::singleton()));
    _opDisp.addOp(ByteCodes::trunc, new InterpFun1(_TRUNC_::singleton()));

    // common functions
    _opDisp.addOp(ByteCodes::clamp, new InterpFun3(_CLAMP_::singleton()));
    _opDisp.addOp(ByteCodes::degrees, new InterpFun1(_DEGREES_::singleton()));
    _opDisp.addOp(ByteCodes::max, new InterpFun2(_MAX_::singleton()));
    _opDisp.addOp(ByteCodes::min, new InterpFun2(_MIN_::singleton()));
    _opDisp.addOp(ByteCodes::mix, new InterpFun3(_MIX_::singleton()));
    _opDisp.addOp(ByteCodes::radians, new InterpFun1(_RADIANS_::singleton()));
    _opDisp.addOp(ByteCodes::step, new InterpFun2(_STEP_::singleton()));
    _opDisp.addOp(ByteCodes::smoothstep,
                  new InterpFun3(_SMOOTHSTEP_::singleton()));
    _opDisp.addOp(ByteCodes::sign, new InterpFun1(_SIGN_::singleton()));

    // relational functions
    _opDisp.addOp(ByteCodes::isequal, new InterpFun2(_ISEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isnotequal,
                  new InterpFun2(_ISNOTEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isgreater,
                  new InterpFun2(_ISGREATER_::singleton()));
    _opDisp.addOp(ByteCodes::isgreaterequal,
                  new InterpFun2(_ISGREATEREQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::isless, new InterpFun2(_ISLESS_::singleton()));
    _opDisp.addOp(ByteCodes::islessequal,
                  new InterpFun2(_ISLESSEQUAL_::singleton()));
    _opDisp.addOp(ByteCodes::islessgreater,
                  new InterpFun2(_ISLESSGREATER_::singleton()));
    _opDisp.addOp(ByteCodes::isfinite, new InterpFun1(_ISFINITE_::singleton()));
    _opDisp.addOp(ByteCodes::isinf, new InterpFun1(_ISINF_::singleton()));
    _opDisp.addOp(ByteCodes::isnan, new InterpFun1(_ISNAN_::singleton()));
    _opDisp.addOp(ByteCodes::isnormal, new InterpFun1(_ISNORMAL_::singleton()));
    _opDisp.addOp(ByteCodes::isordered,
                  new InterpFun2(_ISORDERED_::singleton()));
    _opDisp.addOp(ByteCodes::isunordered,
                  new InterpFun2(_ISUNORDERED_::singleton()));
    _opDisp.addOp(ByteCodes::signbit, new InterpFun1(_SIGNBIT_::singleton()));
}

bool Interpreter::extendLanguage(const uint32_t opCode,
                                 const BaseInterp& opHandler)
{
    _opDisp.deleteOp(opCode); // release old handler, if any
    _opDisp.addOp(opCode, opHandler.clone()); // replace with new handler
    _opDisp.setContext(*_memMgr);

    return true;
}

void Interpreter::sub_initDevice(MemManager& mm)
{
    _memMgr = &mm;
    _opDisp.setContext(mm);
}

bool Interpreter::sub_evaluate(VectorTrace& vt)
{
#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "hashcode=" << vt.hashCode()
       << " numtraces=" << vt.numTraces();
    LOGGER(ss.str())
#endif

    _opDisp.setContext(vt,
                       Scheduler::singleton().swizzleKey());

    // iterate over statements
    for (size_t i = 0; i < vt.traceLength(); i++)
    {
        // print //

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << vt.getVariable(i) << "." << vt.getVersion(i) << "\t";

        PrintBC v(ss, " ");
        vt.getRHS(i).descend(v);

        if (vt.frontMem().count(v.ptrIndex()))
        {
            ss << "( ";

            for (size_t j = 0; j < vt.numTraces(); j++)
            {
                ss << vt.frontMem()[v.ptrIndex()][j] << " ";
            }

            ss << ")";
        }

        LOGGER(ss.str())
#endif

        // calculate //

        stack< vector< FrontMem* > > outStack;

        _opDisp.setContext(outStack);

        vt.getRHS(i).ascend(_opDisp);

        if (! outStack.empty())
        {
            // set LHS variable
            vt.vectorNuts()[vt.getVariable(i)]->setNutMem( vt.getVersion(i),
                                                           outStack.top() );

            // need MT safety
            static_cast< FrontMem* >(outStack.top().front())
                ->getBackMem()
                ->needMTSafe();
        }
    }

    // the interpreter always succeeds
    return true;
}

}; // namespace chai_internal
