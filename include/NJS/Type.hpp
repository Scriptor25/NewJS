#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/Type.h>

namespace NJS
{
    class Builder;

    typedef std::shared_ptr<struct Type> TypePtr;

    std::ostream& operator<<(std::ostream&, const TypePtr&);

    struct Type
    {
        explicit Type(std::string);
        virtual ~Type() = default;

        virtual bool IsTuple();
        virtual size_t Size();
        virtual TypePtr Member(const std::string&);
        virtual size_t MemberIndex(const std::string&);
        virtual TypePtr Result();
        virtual TypePtr Element();
        virtual TypePtr Element(size_t);
        virtual size_t ElementSize();

        virtual llvm::Type* GenLLVM(Builder&) = 0;

        std::ostream& Print(std::ostream&);

        std::string String;
    };

    struct MultiType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        explicit MultiType(const std::vector<TypePtr>&);

        llvm::Type* GenLLVM(Builder&) override;

        std::vector<TypePtr> Types;
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

        TypePtr Element() override;
        TypePtr Element(size_t) override;
        size_t ElementSize() override;

        llvm::Type* GenLLVM(Builder&) override;

        TypePtr ElementType;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        TupleType(TypePtr, std::vector<TypePtr>);

        bool IsTuple() override;
        size_t Size() override;
        TypePtr Element() override;
        TypePtr Element(size_t) override;

        llvm::Type* GenLLVM(Builder&) override;

        TypePtr ElementType;
        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        explicit ObjectType(const std::map<std::string, TypePtr>&);

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
