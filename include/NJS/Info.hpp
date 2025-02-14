#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    struct ValueInfo
    {
        llvm::Value *Val;
        llvm::Value *Ptr;
    };

    template<unsigned N>
    struct OperatorInfo;

    template<>
    struct OperatorInfo<1>
    {
        TypePtr Result;
        TypePtr Value;
        llvm::Value *Callee;
    };

    template<>
    struct OperatorInfo<2>
    {
        TypePtr Result;
        TypePtr Left;
        TypePtr Right;
        llvm::Value *Callee;
    };
}
