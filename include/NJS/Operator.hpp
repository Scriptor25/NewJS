#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    ValuePtr OperatorEQ(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorNE(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLT(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLE(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorGT(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorGE(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    ValuePtr OperatorLOr(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLXor(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorLAnd(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorOr(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorXor(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorAnd(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    ValuePtr OperatorAdd(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorSub(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorMul(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorDiv(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorRem(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorPow(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorShL(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);
    ValuePtr OperatorShR(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs);

    UnaryResult OperatorInc(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorDec(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorNeg(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorLNot(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorNot(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorRef(Builder &builder, const SourceLocation &where, const ValuePtr &value);
    UnaryResult OperatorDeref(Builder &builder, const SourceLocation &where, const ValuePtr &value);
}
