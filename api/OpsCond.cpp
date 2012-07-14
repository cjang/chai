// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "chai/chai.h"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// ternary conditional operation

TriFun cond(const ArrayExp& pred, const ArrayExp& A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const ArrayExp& A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const ArrayExp& A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const uint32_t A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const ArrayExp& B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const uint32_t A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const uint32_t A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const int32_t A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const int32_t A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const float A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const float A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

TriFun cond(const ArrayExp& pred, const double A, const uint32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const int32_t B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const float B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }
TriFun cond(const ArrayExp& pred, const double A, const double B) {
    return TriFun(ByteCodes::operatorCONDEXPR, pred, A, B); }

}; // namespace chai
