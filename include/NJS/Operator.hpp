#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    ValuePtr OperatorEQ(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorNE(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLT(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLE(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorGT(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorGE(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);

    ValuePtr OperatorLOr(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLXor(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorLAnd(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorOr(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorXor(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorAnd(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);

    ValuePtr OperatorAdd(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorSub(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorMul(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorDiv(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorRem(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorPow(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorShL(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);
    ValuePtr OperatorShR(Builder&, const TypePtr&, llvm::Value*, llvm::Value*);

    std::pair<ValuePtr, bool> OperatorInc(Builder&, const ValuePtr&);
    std::pair<ValuePtr, bool> OperatorDec(Builder&, const ValuePtr&);
    std::pair<ValuePtr, bool> OperatorNeg(Builder&, const ValuePtr&);
    std::pair<ValuePtr, bool> OperatorLNot(Builder&, const ValuePtr&);
    std::pair<ValuePtr, bool> OperatorNot(Builder&, const ValuePtr&);
    std::pair<ValuePtr, bool> OperatorRef(Builder&, const ValuePtr&);
}
