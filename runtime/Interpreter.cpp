// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdint.h>

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
#include "PrecType.hpp"
#include "Scheduler.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// Interpreter controller

void Interpreter::op(const uint32_t opCode,
                     BaseInterp* opHandler)
{
    _opDisp.addOp( opCode, opHandler );
}

void Interpreter::internalOps(void)
{
    op(ByteCodes::convert_u32, new InterpConvert(PrecType::UInt32));
    op(ByteCodes::convert_i32, new InterpConvert(PrecType::Int32));
    op(ByteCodes::convert_f32, new InterpConvert(PrecType::Float));
    op(ByteCodes::convert_f64, new InterpConvert(PrecType::Double));
    op(ByteCodes::scalar_u32, new InterpScalar(PrecType::UInt32));
    op(ByteCodes::scalar_i32, new InterpScalar(PrecType::Int32));
    op(ByteCodes::scalar_f32, new InterpScalar(PrecType::Float));
    op(ByteCodes::scalar_f64, new InterpScalar(PrecType::Double));
}

void Interpreter::arithmeticOps(void)
{
    op(ByteCodes::operatorUNARYMINUS, new InterpFun1(_UNARYMINUS_::s()));
    op(ByteCodes::operatorADD,        new InterpFun2(_ADD_::s()));
    op(ByteCodes::operatorDIVIDE,     new InterpFun2(_DIVIDE_::s()));
    op(ByteCodes::operatorMULTIPLY,   new InterpFun2(_MULTIPLY_::s()));
    op(ByteCodes::operatorSUBTRACT,   new InterpFun2(_SUBTRACT_::s()));
    op(ByteCodes::operatorCOMPLEMENT, new InterpFun1(_COMPLEMENT_::s()));
    op(ByteCodes::operatorNOT,        new InterpFun1(_NOT_::s()));
    op(ByteCodes::operatorMODULO,     new InterpFun2(_MODULO_::s()));
    op(ByteCodes::operatorSHIFTLEFT,  new InterpFun2(_SHIFTLEFT_::s()));
    op(ByteCodes::operatorSHIFTRIGHT, new InterpFun2(_SHIFTRIGHT_::s()));
    op(ByteCodes::operatorBITWISEAND, new InterpFun2(_BITWISEAND_::s()));
    op(ByteCodes::operatorBITWISEOR,  new InterpFun2(_BITWISEOR_::s()));
    op(ByteCodes::operatorBITWISEXOR, new InterpFun2(_BITWISEXOR_::s()));
    op(ByteCodes::operatorCONDEXPR,   new InterpCond);
}

void Interpreter::predicateOps(void)
{
    op(ByteCodes::operatorLOGICALAND,   new InterpFun2(_LOGICALAND_::s()));
    op(ByteCodes::operatorLOGICALOR,    new InterpFun2(_LOGICALOR_::s()));
    op(ByteCodes::operatorEQUAL,        new InterpFun2(_EQUAL_::s()));
    op(ByteCodes::operatorNOTEQUAL,     new InterpFun2(_NOTEQUAL_::s()));
    op(ByteCodes::operatorGREATEREQUAL, new InterpFun2(_GREATEREQUAL_::s()));
    op(ByteCodes::operatorGREATERTHAN,  new InterpFun2(_GREATERTHAN_::s()));
    op(ByteCodes::operatorLESSEQUAL,    new InterpFun2(_LESSEQUAL_::s()));
    op(ByteCodes::operatorLESSTHAN,     new InterpFun2(_LESSTHAN_::s()));
}

void Interpreter::gatherOps(void)
{
    op(ByteCodes::gather1_floor, new InterpGather(1));
    op(ByteCodes::gather2_floor, new InterpGather(2));
    op(ByteCodes::index1_u32, new InterpIdxdata(1, PrecType::UInt32));
    op(ByteCodes::index1_i32, new InterpIdxdata(1, PrecType::Int32));
    op(ByteCodes::index1_f32, new InterpIdxdata(1, PrecType::Float));
    op(ByteCodes::index1_f64, new InterpIdxdata(1, PrecType::Double));
    op(ByteCodes::index2_u32, new InterpIdxdata(2, PrecType::UInt32));
    op(ByteCodes::index2_i32, new InterpIdxdata(2, PrecType::Int32));
    op(ByteCodes::index2_f32, new InterpIdxdata(2, PrecType::Float));
    op(ByteCodes::index2_f64, new InterpIdxdata(2, PrecType::Double));
}

void Interpreter::dataOps(void)
{
    op(ByteCodes::make1_u32, new InterpMakedata);
    op(ByteCodes::make1_i32, new InterpMakedata);
    op(ByteCodes::make1_f32, new InterpMakedata);
    op(ByteCodes::make1_f64, new InterpMakedata);
    op(ByteCodes::make2_u32, new InterpMakedata);
    op(ByteCodes::make2_i32, new InterpMakedata);
    op(ByteCodes::make2_f32, new InterpMakedata);
    op(ByteCodes::make2_f64, new InterpMakedata);
    op(ByteCodes::ones1_u32, new InterpLitdata(1, static_cast<uint32_t>(1)));
    op(ByteCodes::ones1_i32, new InterpLitdata(1, static_cast<int32_t>(1)));
    op(ByteCodes::ones1_f32, new InterpLitdata(1, static_cast<float>(1)));
    op(ByteCodes::ones1_f64, new InterpLitdata(1, static_cast<double>(1)));
    op(ByteCodes::ones2_u32, new InterpLitdata(2, static_cast<uint32_t>(1)));
    op(ByteCodes::ones2_i32, new InterpLitdata(2, static_cast<int32_t>(1)));
    op(ByteCodes::ones2_f32, new InterpLitdata(2, static_cast<float>(1)));
    op(ByteCodes::ones2_f64, new InterpLitdata(2, static_cast<double>(1)));
    op(ByteCodes::zeros1_u32, new InterpLitdata(1, static_cast<uint32_t>(0)));
    op(ByteCodes::zeros1_i32, new InterpLitdata(1, static_cast<int32_t>(0)));
    op(ByteCodes::zeros1_f32, new InterpLitdata(1, static_cast<float>(0)));
    op(ByteCodes::zeros1_f64, new InterpLitdata(1, static_cast<double>(0)));
    op(ByteCodes::zeros2_u32, new InterpLitdata(2, static_cast<uint32_t>(0)));
    op(ByteCodes::zeros2_i32, new InterpLitdata(2, static_cast<int32_t>(0)));
    op(ByteCodes::zeros2_f32, new InterpLitdata(2, static_cast<float>(0)));
    op(ByteCodes::zeros2_f64, new InterpLitdata(2, static_cast<double>(0)));
}

void Interpreter::readOps(void)
{
    op(ByteCodes::read_scalar_u32, new InterpReadout);
    op(ByteCodes::read_scalar_i32, new InterpReadout);
    op(ByteCodes::read_scalar_f32, new InterpReadout);
    op(ByteCodes::read_scalar_f64, new InterpReadout);
    op(ByteCodes::read1_u32, new InterpReadout);
    op(ByteCodes::read1_i32, new InterpReadout);
    op(ByteCodes::read1_f32, new InterpReadout);
    op(ByteCodes::read1_f64, new InterpReadout);
    op(ByteCodes::read2_u32, new InterpReadout);
    op(ByteCodes::read2_i32, new InterpReadout);
    op(ByteCodes::read2_f32, new InterpReadout);
    op(ByteCodes::read2_f64, new InterpReadout);
}

void Interpreter::randomOps(void)
{
    const size_t up = PrecType::UInt32;
    const size_t ip = PrecType::Int32;
    const size_t fp = PrecType::Float;
    const size_t dp = PrecType::Double;

    op(ByteCodes::rng_normal_make_f32, new InterpRNGnormal(fp, _genRNG));
    op(ByteCodes::rng_normal_make_f64, new InterpRNGnormal(dp, _genRNG));
    op(ByteCodes::rng_uniform_make_u32, new InterpRNGuniform(up, _genRNG));
    op(ByteCodes::rng_uniform_make_i32, new InterpRNGuniform(ip, _genRNG));
    op(ByteCodes::rng_uniform_make_f32, new InterpRNGuniform(fp, _genRNG));
    op(ByteCodes::rng_uniform_make_f64, new InterpRNGuniform(dp, _genRNG));
}

void Interpreter::matmulOps(void)
{
    op(ByteCodes::matmul, new InterpMatmul);
    op(ByteCodes::matmulG, new InterpMatmulG);
    op(ByteCodes::transpose, new InterpTranspose);
}

void Interpreter::openclOps(void)
{
    // inline OpenCL is not supported by the interpreter
}

void Interpreter::reduceOps(void)
{
    op(ByteCodes::dot_product, new InterpDotprod);
    op(ByteCodes::mean, new InterpAccum(true));
    op(ByteCodes::sum, new InterpAccum(false));
}

void Interpreter::mathOps(void)
{
    op(ByteCodes::acos,      new InterpFun1(_ACOS_::s()));
    op(ByteCodes::acosh,     new InterpFun1(_ACOSH_::s()));
    op(ByteCodes::acospi,    new InterpFun1(_ACOSPI_::s()));
    op(ByteCodes::asin,      new InterpFun1(_ASIN_::s()));
    op(ByteCodes::asinh,     new InterpFun1(_ASINH_::s()));
    op(ByteCodes::asinpi,    new InterpFun1(_ASINPI_::s()));
    op(ByteCodes::atan,      new InterpFun1(_ATAN_::s()));
    op(ByteCodes::atan2,     new InterpFun2(_ATAN2_::s()));
    op(ByteCodes::atanh,     new InterpFun1(_ATANH_::s()));
    op(ByteCodes::atanpi,    new InterpFun1(_ATANPI_::s()));
    op(ByteCodes::atan2pi,   new InterpFun2(_ATAN2PI_::s()));
    op(ByteCodes::cbrt,      new InterpFun1(_CBRT_::s()));
    op(ByteCodes::ceil,      new InterpFun1(_CEIL_::s()));
    op(ByteCodes::copysign,  new InterpFun2(_COPYSIGN_::s()));
    op(ByteCodes::cos,       new InterpFun1(_COS_::s()));
    op(ByteCodes::cosh,      new InterpFun1(_COSH_::s()));
    op(ByteCodes::cospi,     new InterpFun1(_COSPI_::s()));
    op(ByteCodes::erfc,      new InterpFun1(_ERFC_::s()));
    op(ByteCodes::erf,       new InterpFun1(_ERF_::s()));
    op(ByteCodes::exp,       new InterpFun1(_EXP_::s()));
    op(ByteCodes::exp2,      new InterpFun1(_EXP2_::s()));
    op(ByteCodes::exp10,     new InterpFun1(_EXP10_::s()));
    op(ByteCodes::expm1,     new InterpFun1(_EXPM1_::s()));
    op(ByteCodes::fabs,      new InterpFun1(_FABS_::s()));
    op(ByteCodes::fdim,      new InterpFun2(_FDIM_::s()));
    op(ByteCodes::floor,     new InterpFun1(_FLOOR_::s()));
    op(ByteCodes::fma,       new InterpFun3(_FMA_::s()));
    op(ByteCodes::fmax,      new InterpFun2(_FMAX_::s()));
    op(ByteCodes::fmin,      new InterpFun2(_FMIN_::s()));
    op(ByteCodes::fmod,      new InterpFun2(_FMOD_::s()));
    op(ByteCodes::hypot,     new InterpFun2(_HYPOT_::s()));
    op(ByteCodes::ilogb,     new InterpFun1(_ILOGB_::s()));
    op(ByteCodes::ldexp,     new InterpFun2(_LDEXP_::s()));
    op(ByteCodes::lgamma,    new InterpFun1(_LGAMMA_::s()));
    op(ByteCodes::log,       new InterpFun1(_LOG_::s()));
    op(ByteCodes::log2,      new InterpFun1(_LOG2_::s()));
    op(ByteCodes::log10,     new InterpFun1(_LOG10_::s()));
    op(ByteCodes::log1p,     new InterpFun1(_LOG1P_::s()));
    op(ByteCodes::logb,      new InterpFun1(_LOGB_::s()));
    op(ByteCodes::mad,       new InterpFun3(_MAD_::s()));
    op(ByteCodes::maxmag,    new InterpFun2(_MAXMAG_::s()));
    op(ByteCodes::minmag,    new InterpFun2(_MINMAG_::s()));
    op(ByteCodes::nan,       new InterpFun1(_NAN_::s()));
    op(ByteCodes::nextafter, new InterpFun2(_NEXTAFTER_::s()));
    op(ByteCodes::pow,       new InterpFun2(_POW_::s()));
    op(ByteCodes::pown,      new InterpFun2(_POWN_::s()));
    op(ByteCodes::powr,      new InterpFun2(_POWR_::s()));
    op(ByteCodes::remainder, new InterpFun2(_REMAINDER_::s()));
    op(ByteCodes::rint,      new InterpFun1(_RINT_::s()));
    op(ByteCodes::rootn,     new InterpFun2(_ROOTN_::s()));
    op(ByteCodes::round,     new InterpFun1(_ROUND_::s()));
    op(ByteCodes::rsqrt,     new InterpFun1(_RSQRT_::s()));
    op(ByteCodes::sin,       new InterpFun1(_SIN_::s()));
    op(ByteCodes::sinh,      new InterpFun1(_SINH_::s()));
    op(ByteCodes::sinpi,     new InterpFun1(_SINPI_::s()));
    op(ByteCodes::sqrt,      new InterpFun1(_SQRT_::s()));
    op(ByteCodes::tan,       new InterpFun1(_TAN_::s()));
    op(ByteCodes::tanh,      new InterpFun1(_TANH_::s()));
    op(ByteCodes::tanpi,     new InterpFun1(_TANPI_::s()));
    op(ByteCodes::tgamma,    new InterpFun1(_TGAMMA_::s()));
    op(ByteCodes::trunc,     new InterpFun1(_TRUNC_::s()));
}

void Interpreter::commonOps(void)
{
    op(ByteCodes::clamp,      new InterpFun3(_CLAMP_::s()));
    op(ByteCodes::degrees,    new InterpFun1(_DEGREES_::s()));
    op(ByteCodes::max,        new InterpFun2(_MAX_::s()));
    op(ByteCodes::min,        new InterpFun2(_MIN_::s()));
    op(ByteCodes::mix,        new InterpFun3(_MIX_::s()));
    op(ByteCodes::radians,    new InterpFun1(_RADIANS_::s()));
    op(ByteCodes::step,       new InterpFun2(_STEP_::s()));
    op(ByteCodes::smoothstep, new InterpFun3(_SMOOTHSTEP_::s()));
    op(ByteCodes::sign,       new InterpFun1(_SIGN_::s()));
}

void Interpreter::integerOps(void)
{
    op(ByteCodes::abs,      new InterpFun1(_ABS_::s()));
    op(ByteCodes::abs_diff, new InterpFun2(_ABS_DIFF_::s()));
    op(ByteCodes::add_sat,  new InterpFun2(_ADD_SAT_::s()));
    op(ByteCodes::clz,      new InterpFun1(_CLZ_::s()));
    op(ByteCodes::hadd,     new InterpFun2(_HADD_::s()));
    op(ByteCodes::mad24,    new InterpFun3(_MAD24_::s()));
    op(ByteCodes::mad_hi,   new InterpFun3(_MAD_HI_::s()));
    op(ByteCodes::mad_sat,  new InterpFun3(_MAD_SAT_::s()));
    op(ByteCodes::mul24,    new InterpFun2(_MUL24_::s()));
    op(ByteCodes::mul_hi,   new InterpFun2(_MUL_HI_::s()));
    op(ByteCodes::rhadd,    new InterpFun2(_RHADD_::s()));
    op(ByteCodes::rotate,   new InterpFun2(_ROTATE_::s()));
    op(ByteCodes::sub_sat,  new InterpFun2(_SUB_SAT_::s()));
}

void Interpreter::relationalOps(void)
{
    op(ByteCodes::isequal,        new InterpFun2(_ISEQUAL_::s()));
    op(ByteCodes::isnotequal,     new InterpFun2(_ISNOTEQUAL_::s()));
    op(ByteCodes::isgreater,      new InterpFun2(_ISGREATER_::s()));
    op(ByteCodes::isgreaterequal, new InterpFun2(_ISGREATEREQUAL_::s()));
    op(ByteCodes::isless,         new InterpFun2(_ISLESS_::s()));
    op(ByteCodes::islessequal,    new InterpFun2(_ISLESSEQUAL_::s()));
    op(ByteCodes::islessgreater,  new InterpFun2(_ISLESSGREATER_::s()));
    op(ByteCodes::isfinite,       new InterpFun1(_ISFINITE_::s()));
    op(ByteCodes::isinf,          new InterpFun1(_ISINF_::s()));
    op(ByteCodes::isnan,          new InterpFun1(_ISNAN_::s()));
    op(ByteCodes::isnormal,       new InterpFun1(_ISNORMAL_::s()));
    op(ByteCodes::isordered,      new InterpFun2(_ISORDERED_::s()));
    op(ByteCodes::isunordered,    new InterpFun2(_ISUNORDERED_::s()));
    op(ByteCodes::signbit,        new InterpFun1(_SIGNBIT_::s()));
}

Interpreter::Interpreter(const int deviceNum)
    : DeviceBase(deviceNum),
      _genRNG()
{
    internalOps();
    arithmeticOps();
    predicateOps();
    gatherOps();
    dataOps();
    readOps();
    randomOps();
    matmulOps();
    //openclOps();
    reduceOps();
    mathOps();
    commonOps();
    integerOps();
    relationalOps();
}

bool Interpreter::extendLanguage(const uint32_t opCode,
                                 const BaseInterp& opHandler)
{
    BaseInterp* cloneHandler = opHandler.clone();

    if (cloneHandler)
    {
        cloneHandler->setContext(getMemInterp());

        _opDisp.deleteOp(opCode); // release old handler, if any

        op(opCode, cloneHandler); // replace with new handler

        return true;
    }
    else
    {
        return false;
    }
}

void Interpreter::sub_initDevice(MemInterp& mm)
{
    _opDisp.setContext(mm);
}

bool Interpreter::sub_evaluate(VectorTrace& vt)
{
    _opDisp.setContext(vt, Scheduler::singleton().swizzleKey());

    // iterate over statements
    for (size_t i = 0; i < vt.traceLength(); i++)
    {
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
