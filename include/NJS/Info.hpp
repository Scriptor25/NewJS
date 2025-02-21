#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    struct ValueInfo
    {
        llvm::Value *Val{};
        llvm::Value *Ptr{};
    };

    template<unsigned N>
    struct OperatorInfo;

    template<>
    struct OperatorInfo<1>
    {
        TypePtr ResultType;
        TypePtr ValueType;
        llvm::Value *Callee{};
    };

    template<>
    struct OperatorInfo<2>
    {
        TypePtr ResultType;
        TypePtr LeftType;
        TypePtr RightType;
        llvm::Value *Callee{};
    };

    struct MemberInfo
    {
        unsigned Index;
        std::string Name;
        TypePtr Type;
    };
}
