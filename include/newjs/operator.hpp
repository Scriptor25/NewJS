#pragma once

#include <functional>
#include <newjs/newjs.hpp>

namespace NJS
{
    using BinaryOperator = std::function<ValuePtr(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right)>;

    using UnaryOperator = std::function<ValuePtr(
        const Builder &builder,
        const ValuePtr &value)>;

    ValuePtr OperatorEQ(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorNE(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLT(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLE(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorGT(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorGE(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorLOr(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLXor(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorLAnd(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorOr(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorXor(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorAnd(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorAdd(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorSub(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorMul(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorDiv(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorRem(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorPow(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorShL(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);
    ValuePtr OperatorShR(
        const Builder &builder,
        const ValuePtr &left,
        const ValuePtr &right);

    ValuePtr OperatorInc(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorDec(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorNeg(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorLNot(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorNot(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorRef(const Builder &builder, const ValuePtr &operand);
    ValuePtr OperatorDeref(const Builder &builder, const ValuePtr &operand);
}
