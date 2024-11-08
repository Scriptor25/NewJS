#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/Type.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct Type
    {
        explicit Type(std::string);
        virtual ~Type() = default;

        virtual bool IsTuple();
        virtual bool IsComplex();
        virtual size_t Size();
        virtual TypePtr Member(const std::string&);
        virtual size_t MemberIndex(const std::string&);
        virtual TypePtr Result();
        virtual TypePtr Element();
        virtual TypePtr Element(size_t);
        virtual size_t ElementSize();

        virtual llvm::Type* GenLLVM(Builder&) = 0;

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

        size_t Size() override;

        llvm::Type* GenLLVM(Builder&) override;

        TypeName Name;
    };

    struct ArrayType : Type
    {
        static std::string GenString(const TypePtr&);

        explicit ArrayType(TypePtr);

        bool IsComplex() override;
        TypePtr Element() override;
        TypePtr Element(size_t) override;
        size_t ElementSize() override;

        llvm::Type* GenLLVM(Builder&) override;

        TypePtr ElementType;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        explicit TupleType(std::vector<TypePtr>);

        bool IsComplex() override;
        bool IsTuple() override;
        size_t Size() override;
        TypePtr Element(size_t) override;

        llvm::Type* GenLLVM(Builder&) override;

        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        explicit ObjectType(const std::map<std::string, TypePtr>&);

        bool IsComplex() override;
        size_t Size() override;
        TypePtr Member(const std::string&) override;
        size_t MemberIndex(const std::string&) override;

        llvm::Type* GenLLVM(Builder&) override;

        std::vector<std::pair<std::string, TypePtr>> ElementTypes;
    };

    struct FunctionType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&, const TypePtr&, bool);

        FunctionType(std::vector<TypePtr>, TypePtr, bool);

        bool IsComplex() override;
        TypePtr Result() override;

        llvm::Type* GenLLVM(Builder&) override;

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
