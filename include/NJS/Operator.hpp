#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    ValuePtr OperatorEQ(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorNE(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLT(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLE(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorGT(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorGE(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);

    ValuePtr OperatorLOr(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLXor(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLAnd(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorOr(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorXor(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorAnd(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);

    ValuePtr OperatorAdd(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorSub(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorMul(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorDiv(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorRem(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorPow(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorShL(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorShR(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*);

    struct UnaryResult
    {
        ValuePtr Value{};
        bool Assign{};
    };

    UnaryResult OperatorInc(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorDec(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorNeg(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorLNot(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorNot(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorRef(Builder&, const SourceLocation&, const ValuePtr&);
    UnaryResult OperatorDeref(Builder&, const SourceLocation&, const ValuePtr&);
}
