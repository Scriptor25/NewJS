#pragma once

#include <newjs/newjs.hpp>

namespace NJS
{
    using BinaryOperator = std::function<ValuePtr(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right)>;

    using UnaryOperator = std::function<ValuePtr(
        Builder &builder,
        const ValuePtr &value)>;

    ValuePtr OperatorEQ(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorNE(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLT(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLE(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorGT(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorGE(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorLOr(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLXor(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLAnd(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorOr(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorXor(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorAnd(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorAdd(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorSub(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorMul(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorDiv(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorRem(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorPow(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorShL(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorShR(
        Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorInc(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorDec(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorNeg(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorLNot(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorNot(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorRef(Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorDeref(Builder &builder, const ValuePtr &operand);
}
