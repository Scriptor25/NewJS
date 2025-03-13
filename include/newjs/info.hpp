#pragma once

#include <newjs/error.hpp>
#include <newjs/newjs.hpp>

namespace llvm
{
    class Value;
    class Type;
}

namespace NJS
{
    struct ValueInfo
    {
        llvm::Value *Val{};
        llvm::Value *Ptr{};
    };

    bool operator==(const ReferenceInfo &a, const ReferenceInfo &b);
    bool operator!=(const ReferenceInfo &a, const ReferenceInfo &b);

    struct ReferenceInfo
    {
        ReferenceInfo() = default;
        ReferenceInfo(TypePtr type, bool is_const, bool is_reference);

        explicit ReferenceInfo(TypePtr type);

        [[nodiscard]] std::string GetString() const;
        [[nodiscard]] size_t GetHash() const;
        [[nodiscard]] llvm::Type *GetLLVM(const Builder &builder) const;

        llvm::Value *SolveFor(const Builder &builder, ValuePtr value) const;

        std::ostream &Print(std::ostream &stream) const;

        TypePtr Type;
        bool IsConst = false;
        bool IsReference = false;
    };

    template<typename V>
    class ReferenceInfoMap
    {
    public:
        ReferenceInfoMap() = default;
        ~ReferenceInfoMap() = default;

        V &operator[](const ReferenceInfo &key)
        {
            for (auto &[key_, value_]: m_Data)
                if (key_ == key)
                    return value_;
            return m_Data.emplace_back(key, V{}).second;
        }

        V &at(const ReferenceInfo &key)
        {
            for (auto &[key_, value_]: m_Data)
                if (key_ == key)
                    return value_;
            return m_Data.emplace_back(key, V{}).second;
        }

        const V &operator[](const ReferenceInfo &key) const
        {
            for (auto &[key_, value_]: m_Data)
                if (key_ == key)
                    return value_;
            Error("key missing in map");
        }

        const V &at(const ReferenceInfo &key) const
        {
            for (auto &[key_, value_]: m_Data)
                if (key_ == key)
                    return value_;
            Error("key missing in map");
        }

        [[nodiscard]] bool contains(const ReferenceInfo &key) const
        {
            for (auto &[key_, value_]: m_Data)
                if (key_ == key)
                    return true;
            return false;
        }

    private:
        std::vector<std::pair<ReferenceInfo, V>> m_Data;
    };

    struct MemberInfo
    {
        unsigned Index;
        std::string Name;
        ReferenceInfo Info;
        ExpressionPtr Default;
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
