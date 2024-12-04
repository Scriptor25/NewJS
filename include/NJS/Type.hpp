#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct Type
    {
        explicit Type(std::string);
        virtual ~Type() = default;

        virtual bool IsPrimitive() const;
        virtual size_t Size() const;
        virtual TypePtr Member(const std::string&);
        virtual size_t MemberIndex(const std::string&);
        virtual TypePtr Result();
        virtual TypePtr Element();
        virtual TypePtr Element(size_t);
        virtual size_t ElementSize();

        virtual void TypeInfo(Builder&, std::vector<llvm::Value*>&) const = 0;
        virtual llvm::Type* GenLLVM(Builder&) const = 0;

        std::ostream& Print(std::ostream&) const;

        std::string String;
    };

    enum TypeName
    {
        Type_Void,
        Type_Boolean,
        Type_Number,
        Type_String,
    };

    struct PrimitiveType : Type
    {
        static std::string GenString(TypeName);

        explicit PrimitiveType(TypeName);

        bool IsPrimitive() const override;
        size_t Size() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        llvm::Type* GenLLVM(Builder&) const override;

        TypeName Name;
    };

    struct ArrayType : Type
    {
        static std::string GenString(const TypePtr&, size_t);

        ArrayType(TypePtr, size_t);

        TypePtr Element() override;
        TypePtr Element(size_t) override;
        size_t ElementSize() override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        llvm::Type* GenLLVM(Builder&) const override;

        TypePtr ElementType;
        size_t ElementCount;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        explicit TupleType(std::vector<TypePtr>);

        size_t Size() const override;
        TypePtr Element(size_t) override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        llvm::Type* GenLLVM(Builder&) const override;

        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        explicit ObjectType(const std::map<std::string, TypePtr>&);

        size_t Size() const override;
        TypePtr Member(const std::string&) override;
        size_t MemberIndex(const std::string&) override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        llvm::Type* GenLLVM(Builder&) const override;

        std::vector<std::pair<std::string, TypePtr>> ElementTypes;
    };

    struct FunctionType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&, const TypePtr&, bool);

        FunctionType(std::vector<TypePtr>, TypePtr, bool);

        TypePtr Result() override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        llvm::Type* GenLLVM(Builder&) const override;

        llvm::FunctionType* GenFnLLVM(Builder&) const;

        std::vector<TypePtr> ParamTypes;
        TypePtr ResultType;
        bool VarArg;
    };
}

namespace std
{
    template <>
    struct formatter<NJS::TypePtr> : formatter<string>
    {
        template <typename FormatContext>
        auto format(const NJS::TypePtr& type, FormatContext& ctx) const
        {
            return formatter<string>::format(type->String, ctx);
        }
    };
}
