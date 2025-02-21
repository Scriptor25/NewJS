#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    struct ValueInfo
    {
        llvm::Value *Val{};
        llvm::Value *Ptr{};
    };

    bool operator==(const ReferenceInfo &a, const ReferenceInfo &b);
    bool operator!=(const ReferenceInfo &a, const ReferenceInfo &b);
    bool operator<(const ReferenceInfo &a, const ReferenceInfo &b);

    struct ReferenceInfo
    {
        ReferenceInfo() = default;
        ReferenceInfo(const TypePtr &type, bool is_const, bool is_reference);

        explicit ReferenceInfo(const TypePtr &type);

        std::string GetString() const;
        llvm::Type *GetLLVM(const SourceLocation &where, const Builder &builder) const;
        unsigned GetSize() const;

        std::ostream &Print(std::ostream &stream) const;

        TypePtr Type;
        bool IsConst = false;
        bool IsReference = false;
    };

    struct MemberInfo
    {
        unsigned Index;
        std::string Name;
        TypePtr Type;
    };

    template<>
    struct OperatorInfo<1>
    {
        ReferenceInfo Result;
        ReferenceInfo Value;
        llvm::Value *Callee{};
    };

    template<>
    struct OperatorInfo<2>
    {
        ReferenceInfo Result;
        ReferenceInfo Left;
        ReferenceInfo Right;
        llvm::Value *Callee{};
    };
}
