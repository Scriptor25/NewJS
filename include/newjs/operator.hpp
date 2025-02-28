#pragma once

#include <newjs/newjs.hpp>

namespace NJS
{
    using BinaryOperator = std::function<ValuePtr(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs)>;

    using UnaryOperator = std::function<ValuePtr(
        Builder &builder,
        const ValuePtr &value)>;

    ValuePtr OperatorEQ(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorNE(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLT(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLE(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorGT(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorGE(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    ValuePtr OperatorLOr(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLXor(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLAnd(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorOr(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorXor(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorAnd(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    ValuePtr OperatorAdd(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorSub(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorMul(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorDiv(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorRem(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorPow(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorShL(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorShR(
        Builder &builder,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    ValuePtr OperatorInc(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorDec(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorNeg(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorLNot(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorNot(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorRef(Builder &builder, const ValuePtr &value);
    ValuePtr OperatorDeref(Builder &builder, const ValuePtr &value);
}
